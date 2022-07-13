#include "trpch.h"
#include "ScriptUtils.h"
#include "ScriptMarshal.h"

#include "Core/Log.h" 

#include <mono/metadata/object.h>
#include <mono/metadata/class.h>
#include <mono/metadata/appdomain.h>

namespace TerranEngine
{
    void ScriptUtils::PrintUnhandledException(MonoException* monoException)
    {
        if(!monoException)
            return;
        
        MonoClass* exceptionClass = mono_object_get_class((MonoObject*)monoException);
        std::string className = mono_class_get_name(exceptionClass);
        MonoProperty* messageProp = mono_class_get_property_from_name(exceptionClass, "Message");
        MonoString* messageStr = (MonoString*)mono_property_get_value(messageProp, (MonoObject*)monoException, nullptr, nullptr);
        TR_CLIENT_ERROR(fmt::format("{0}: {1}", className, ScriptMarshal::MonoStringToUTF8(messageStr)));
    }

    void ScriptUtils::SetFieldDataRaw(void* value, MonoClassField* monoField, GCHandle handle)
    {
        MonoObject* monoObject = GCManager::GetManagedObject(handle);
        TR_ASSERT(monoObject, "Invalid Object!");
        TR_ASSERT(monoField, "Invalid Field!");

        mono_field_set_value(monoObject, monoField, value);
    }
    
    void ScriptUtils::GetFieldDataRaw(void* result, MonoClassField* monoField, GCHandle handle)
    {
        MonoObject* monoObject = GCManager::GetManagedObject(handle);
        TR_ASSERT(monoObject, "Invalid Object!");
        TR_ASSERT(monoField, "Invalid Field!");

        mono_field_get_value(monoObject, monoField, result);
    }

    MonoObject* ScriptUtils::GetFieldValueObject(MonoClassField* monoField, GCHandle handle)
    {
        MonoObject* monoObject = GCManager::GetManagedObject(handle);
        TR_ASSERT(monoObject, "Invalid Object!");
        TR_ASSERT(monoField, "Invalid Field!");

        return mono_field_get_value_object(mono_domain_get(), monoField, monoObject);
    }
}
