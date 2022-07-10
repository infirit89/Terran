#include "trpch.h"
#include "ScriptUtils.h"
#include "ScriptMarshal.h"

#include "Core/Log.h" 

#include <mono/metadata/object.h>
#include <mono/metadata/class.h>

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
}
