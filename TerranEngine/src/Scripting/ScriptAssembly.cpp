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
    { }

    ManagedClass ScriptAssembly::GetClassFromName(const std::string& moduleName)
    {
        TR_ASSERT(m_MonoImage, "Mono Image not loaded");

        const size_t dotPosition = moduleName.find_last_of(".");

        ManagedClass scriptClass = ManagedClass();

        const std::string& namespaceName = moduleName.substr(0, dotPosition);
        const std::string& className = moduleName.substr(dotPosition + 1);

        MonoClass* klass = mono_class_from_name(m_MonoImage, namespaceName.c_str(), className.c_str());
        if (klass)
        {
            scriptClass = ManagedClass(klass);
            scriptClass.m_Namespace = mono_class_get_namespace(klass);
            scriptClass.m_ClassName = mono_class_get_name(klass);
        }

        return scriptClass;
    }

    ManagedClass ScriptAssembly::GetClassFromTypeToken(uint32_t typeToken)
    {
        TR_ASSERT(m_MonoImage, "Mono Image not loaded");

        ManagedClass klass;
        MonoClass* monoClass = mono_class_get(m_MonoImage, typeToken);
        if(monoClass)
        {
            klass = monoClass;
            klass.m_Namespace = mono_class_get_namespace(monoClass);
            klass.m_ClassName = mono_class_get_name(monoClass);
        }
        
        return klass;
    }

    Shared<ScriptAssembly> ScriptAssembly::LoadAssembly(const std::filesystem::path& assemblyPath)
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

            TR_INFO("Successfuly loaded: {0}", assemblyPath);
        }
        return scriptAssembly;
    }

    ManagedMethod ScriptAssembly::GetMethodFromDesc(const std::string& methodDesc)
    {
        ManagedMethod method;
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
        method = ManagedMethod(monoMethod);
        return method;
    }
}
