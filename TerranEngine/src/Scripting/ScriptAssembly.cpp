#include "trpch.h"
#include "ScriptAssembly.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/image.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/tokentype.h>

namespace TerranEngine 
{
    ScriptAssembly::ScriptAssembly(MonoAssembly* monoAssembly)
        : m_MonoAssembly(monoAssembly), m_MonoImage(mono_assembly_get_image(monoAssembly))
    {
    }

    Shared<AssemblyInfo> ScriptAssembly::GenerateAssemblyInfo()
    {
        Shared<AssemblyInfo> info = CreateShared<AssemblyInfo>();

        const MonoTableInfo* typedefTableInfo = mono_image_get_table_info(m_MonoImage, MONO_TABLE_TYPEDEF);
        const MonoTableInfo* methodTableInfo = mono_image_get_table_info(m_MonoImage, MONO_TABLE_METHOD);
        const MonoTableInfo* fieldTableInfo = mono_image_get_table_info(m_MonoImage, MONO_TABLE_FIELD);
        
        const int typedefTableRows = mono_table_info_get_rows(typedefTableInfo);
        const int methodTableRows = mono_table_info_get_rows(methodTableInfo);
        const int fieldTableRows = mono_table_info_get_rows(fieldTableInfo);
        
        for (int i = 1; i < typedefTableRows; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typedefTableInfo, i, cols, MONO_TYPEDEF_SIZE);

            uint32_t typeToken = (i + 1) | MONO_TOKEN_TYPE_DEF;
            
            std::string namespaceName = mono_metadata_string_heap(m_MonoImage, cols[MONO_TYPEDEF_NAMESPACE]);
            std::string className = mono_metadata_string_heap(m_MonoImage, cols[MONO_TYPEDEF_NAME]);
            
            uint32_t nextCol[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(typedefTableInfo, std::min(i + 1, typedefTableRows - 1), nextCol, MONO_TYPEDEF_SIZE);

            std::string moduleName = fmt::format("{0}.{1}", namespaceName, className);
            for (size_t j = cols[MONO_TYPEDEF_METHOD_LIST] - 1; j
                < std::min(nextCol[MONO_TYPEDEF_METHOD_LIST], (uint32_t)methodTableRows) - 1; j++)
            {
                uint32_t methodCols[MONO_METHOD_SIZE];

                mono_metadata_decode_row(methodTableInfo, j, methodCols, MONO_METHOD_SIZE);
                std::string methodName = mono_metadata_string_heap(m_MonoImage, methodCols[MONO_METHOD_NAME]);

                
                const char* blob = mono_metadata_blob_heap(m_MonoImage, methodCols[MONO_METHOD_SIGNATURE]);
                
                const char* c;
                uint32_t val = mono_metadata_decode_value(blob, &c);
                const char* cc;
                MonoMethodSignature* signature = mono_metadata_parse_method_signature(m_MonoImage, val, c, &cc);
                
                char* methodDesc = mono_signature_get_desc(signature, false);
                std::string formattedMethodDesc = fmt::format(":{0}({1})", methodName, methodDesc);
                
                info->MethodInfoMap[moduleName].emplace_back(std::move(formattedMethodDesc));

                mono_metadata_free_method_signature(signature);
            }

            for (size_t j = cols[MONO_TYPEDEF_FIELD_LIST] - 1; j <
                std::min(nextCol[MONO_TYPEDEF_FIELD_LIST], (uint32_t)fieldTableRows) - 1; j++)
            {
                uint32_t fieldCols[MONO_FIELD_SIZE];
                mono_metadata_decode_row(fieldTableInfo, j, fieldCols, MONO_FIELD_SIZE);
                uint32_t fieldTypeToken = (j + 1) | MONO_TOKEN_FIELD_DEF;
                info->FieldInfoMap[moduleName].emplace_back(fieldTypeToken);
            }
            
            info->ClassInfoMap[namespaceName].emplace_back(typeToken);
        }

        info->CurrentImage = m_MonoImage;

        return info;
    }

    ScriptClass ScriptAssembly::GetClassFromName(const std::string& moduleName)
    {
        TR_ASSERT(m_MonoImage, "Mono Image not loaded");

        const size_t dotPosition = moduleName.find_last_of(".");

        ScriptClass scriptClass = ScriptClass();

        const std::string& namespaceName = moduleName.substr(0, dotPosition);
        const std::string& className = moduleName.substr(dotPosition + 1);

        MonoClass* klass = mono_class_from_name(m_MonoImage, namespaceName.c_str(), className.c_str());
        if (klass)
        {
            scriptClass = ScriptClass(klass);
            scriptClass.m_Namespace = mono_class_get_namespace(klass);
            scriptClass.m_ClassName = mono_class_get_name(klass);
        }

        return scriptClass;
    }

    ScriptClass ScriptAssembly::GetClassFromTypeToken(uint32_t typeToken)
    {
        TR_ASSERT(m_MonoImage, "Mono Image not loaded");

        ScriptClass klass;
        MonoClass* monoClass = mono_class_get(m_MonoImage, typeToken);
        if(monoClass)
        {
            klass = ScriptClass(klass);
            klass.m_Namespace = mono_class_get_namespace(monoClass);
            klass.m_ClassName = mono_class_get_name(monoClass);
        }
        
        return klass;
    }

    Shared<ScriptAssembly> ScriptAssembly::LoadScriptAssembly(const std::filesystem::path& assemblyPath)
    {
        FileData* assemblyData = File::OpenFile(assemblyPath.string().c_str());
        Shared<ScriptAssembly> scriptAssembly = nullptr;
        
        if(assemblyData->Data)
        {
            MonoImageOpenStatus status = MonoImageOpenStatus::MONO_IMAGE_OK;
            MonoImage* assemblyImage = mono_image_open_from_data(assemblyData->Data, assemblyData->Length, true, &status);
            
            TR_ASSERT(status == MonoImageOpenStatus::MONO_IMAGE_OK, "Couldn't open the image in location: {0}", assemblyPath.string());

            status = MonoImageOpenStatus::MONO_IMAGE_OK;
            MonoAssembly* assembly = mono_assembly_load_from(assemblyImage, assemblyPath.string().c_str(), &status);
            TR_ASSERT(status == MonoImageOpenStatus::MONO_IMAGE_OK, "Couldn't load the mono assembly in location: {0}", assemblyPath.string());
                
            scriptAssembly = CreateShared<ScriptAssembly>(assembly);

            mono_image_close(assemblyImage);
            File::CloseFile(assemblyData);
            
        }
        return scriptAssembly;
    }

    ScriptMethod ScriptAssembly::GetMethodFromDesc(const std::string& methodDesc)
    {
        ScriptMethod method;
        MonoMethodDesc* monoDesc = mono_method_desc_new(methodDesc.c_str(), false);
        if (!monoDesc)
        {
            TR_ERROR("Couldn't find a matching description ({0}) in the image", methodDesc);
            return method;
        }
        MonoMethod* monoMethod = mono_method_desc_search_in_image(monoDesc, m_MonoImage);
        if (!monoMethod)
        {
            TR_ERROR("Couldn't find the method with signature: {0} in image", methodDesc);
            return method;
        }
        
        mono_method_desc_free(monoDesc);
        method = ScriptMethod(monoMethod);
        return method;
    }
}
