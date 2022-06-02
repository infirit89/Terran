#include "trpch.h"
#include "ScriptAssembly.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/metadata/image.h>

namespace TerranEngine 
{
    ScriptAssembly::ScriptAssembly(MonoAssembly* monoAssembly)
        : m_MonoAssembly(monoAssembly), m_MonoImage(mono_assembly_get_image(monoAssembly))
    {
    }

    Shared<AssemblyInfo> ScriptAssembly::GenerateAssemblyInfo()
    {
        Shared<AssemblyInfo> info = CreateShared<AssemblyInfo>();

        const MonoTableInfo* tableInfo = mono_image_get_table_info(m_MonoImage, MONO_TABLE_TYPEDEF);
        int rows = mono_table_info_get_rows(tableInfo);

        for (size_t i = 0; i < rows; i++)
        {
            uint32_t cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(tableInfo, i, cols, MONO_TYPEDEF_SIZE);

            std::string namespaceName = mono_metadata_string_heap(m_MonoImage, cols[MONO_TYPEDEF_NAMESPACE]);
            std::string className = mono_metadata_string_heap(m_MonoImage, cols[MONO_TYPEDEF_NAME]);


            info->ClassInfoMap[namespaceName].emplace_back(std::move(className));
        }

        info->CurrentImage = m_MonoImage;

        return info;
    }

    ScriptClass* ScriptAssembly::GetClassFromName(const std::string& moduleName)
    {
        TR_ASSERT(m_MonoImage, "Mono Image not loaded");

        size_t dotPosition = moduleName.find_last_of(".");

        ScriptClass* scriptClass = nullptr;

        std::string& namespaceName = moduleName.substr(0, dotPosition);
        std::string& className = moduleName.substr(dotPosition + 1);

        MonoClass* klass = mono_class_from_name(m_MonoImage, namespaceName.c_str(), className.c_str());

        if (klass)
            scriptClass = new ScriptClass(klass);

        return scriptClass;
    }

    Shared<ScriptAssembly> ScriptAssembly::LoadScriptAssembly(const std::filesystem::path& assemblyPath)
    {
        FileData* assemblyData = File::OpenFile(assemblyPath.string().c_str());
        
        Shared<ScriptAssembly> scriptAssembly = nullptr;
        
        MonoImageOpenStatus status = MonoImageOpenStatus::MONO_IMAGE_OK;
        MonoImage* assemblyImage = mono_image_open_from_data(assemblyData->Data, assemblyData->Length, true, &status);
        
        TR_ASSERT(status == MonoImageOpenStatus::MONO_IMAGE_OK, "Couldn't open the image in location: {0}", assemblyPath.string());

        status = MonoImageOpenStatus::MONO_IMAGE_OK;
        MonoAssembly* assembly = mono_assembly_load_from(assemblyImage, assemblyPath.string().c_str(), &status);
        TR_ASSERT(status == MonoImageOpenStatus::MONO_IMAGE_OK, "Couldn't load the mono assembly in location: {0}", assemblyPath.string());
            
        scriptAssembly = CreateShared<ScriptAssembly>(assembly);

        mono_image_close(assemblyImage);
        File::CloseFile(assemblyData);

        return scriptAssembly;
    }
}
