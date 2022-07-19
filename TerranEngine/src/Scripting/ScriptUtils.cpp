#include "trpch.h"
#include "ScriptUtils.h"
#include "ScriptMarshal.h"
#include "ScriptObject.h"
#include "ScriptCache.h"
#include "ScriptClass.h"

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

    void ScriptUtils::SetFieldDataString(const char* value, GCHandle handle, MonoClassField* monoField)
    {
        // if (m_Type.TypeEnum != ScriptType::String)
		// {
		// 	TR_ERROR("Can't set the string value of a non-string field");
		// 	return;
		// }
		
		MonoString* monoStr = ScriptMarshal::UTF8ToMonoString(value);
		ScriptUtils::SetFieldDataRaw(monoStr, monoField, handle);
    }
    
    std::string ScriptUtils::GetFieldDataString(GCHandle handle, MonoClassField* monoField)
    {
        // if (m_Type.TypeEnum != ScriptType::String) 
		// {
		// 	TR_ERROR("Can't get the string value of a non-string field");
		// 	return "";
		// }

		MonoString* monoStr = nullptr;
		ScriptUtils::GetFieldDataRaw(&monoStr, monoField, handle);
		const std::string val = ScriptMarshal::MonoStringToUTF8(monoStr);
		return val;
    }

    void ScriptUtils::SetFieldDataUUID(UUID value, GCHandle handle, MonoClassField* monoField) 
    {
        // if (m_Type.TypeEnum != ScriptType::Entity)
		// {
		// 	TR_ERROR("Field isn't of type 'Entity'");
		// 	return;
		// }

		const ScriptArray uuidArray = ScriptMarshal::UUIDToMonoArray(value);
		
		void* args[] = { uuidArray.GetMonoArray() };
		ScriptObject entityObj = ScriptObject::CreateInstace(*TR_API_CACHED_CLASS(Entity));
		ScriptMethod* constructor = ScriptCache::GetCachedMethod("Terran.Entity", ":.ctor(byte[])");
		constructor->Invoke(entityObj, args);
		
		ScriptUtils::SetFieldDataRaw(entityObj.GetMonoObject(), monoField, handle);
    }

    UUID ScriptUtils::GetFieldDataUUID(GCHandle handle, MonoClassField* monoField) 
    {
        // if (m_Type.TypeEnum != ScriptType::Entity)
		// {
		// 	TR_ERROR("Field isn't of type 'Entity'");
		// 	return {};
		// }
		
		ScriptObject entityObj = ScriptUtils::GetFieldValueObject(monoField, handle);
		ScriptMethod* getIdMethod = ScriptCache::GetCachedMethod("Terran.Entity", ":get_ID");

		if(!entityObj)
			return {};
		
		ScriptObject idObj = getIdMethod->Invoke(entityObj, nullptr);

		if(!idObj)
			return {};
		
		ScriptMethod* getDataMethod = ScriptCache::GetCachedMethod("Terran.UUID", ":get_Data");
		MonoObject* result  = getDataMethod->Invoke(idObj, nullptr).GetMonoObject();
		ScriptArray idDataArr = ScriptArray::Create((MonoArray*)result);
		UUID id = ScriptMarshal::MonoArrayToUUID(idDataArr);
		
		return id;
    }
}
