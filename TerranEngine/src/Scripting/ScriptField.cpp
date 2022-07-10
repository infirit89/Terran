#include "trpch.h"
#include "ScriptField.h"
#include "ScriptMarshal.h"
#include "ScriptCache.h"
#include "ScriptArray.h"
#include "ScriptMethodThunks.h"
#include "ScriptObject.h"
#include "ScriptEngine.h"

#include <mono/metadata/attrdefs.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>

namespace TerranEngine 
{
	static ScriptFieldVisibility ConvertFieldVisibility(uint32_t accessMask) 
	{
		switch (accessMask)
		{
		case MONO_FIELD_ATTR_PRIVATE:	return ScriptFieldVisibility::Private;
		case MONO_FIELD_ATTR_PUBLIC:	return ScriptFieldVisibility::Public;
		case MONO_FIELD_ATTR_FAMILY:	return ScriptFieldVisibility::Protected;
		case MONO_FIELD_ATTR_ASSEMBLY:	return ScriptFieldVisibility::Internal;
		}

		return ScriptFieldVisibility::Unknown;
	}

	ScriptField::ScriptField(MonoClassField* monoField)
		: m_MonoField(monoField)
	{
		m_Name = mono_field_get_name(m_MonoField);
		m_Type = ScriptType(mono_field_get_type(m_MonoField));

		uint32_t accessMask = mono_field_get_flags(m_MonoField) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;
		m_FieldVisibility = ConvertFieldVisibility(accessMask);

		if (m_Type.IsArray())
			m_Type = m_Type.GetElementType();
	}

	void ScriptField::CopyData(GCHandle from, GCHandle to)
	{
		if(m_Type.IsArray())
		{
			ScriptArray fromArr = GetArray(from);
			ScriptArray toArr = GetArray(to);

			if(fromArr.Length() != toArr.Length())
				toArr.Resize(fromArr.Length());

			for (uint32_t i = 0; i < fromArr.Length(); i++)
				toArr.Set(i, fromArr[i]);

			SetArray(toArr, to);

			return;
		}
			
		const auto& val = GetData<Utils::Variant>(from);
		SetData<Utils::Variant>(val, to);
	}

	void ScriptField::SetDataRaw(void* value, GCHandle handle)
	{
		MonoObject* monoObject = GCManager::GetManagedObject(handle);
		if (monoObject == nullptr) 
		{
			TR_ERROR("Couldn't set the value of field {0} because the object that it corresponds to is no longer valid", m_Name);
			return;
		}

		if (m_MonoField == nullptr)
		{
			TR_ERROR("Couldn't set the value of field {0} because it wasn't found", m_Name);
			return;
		}
		
		mono_field_set_value(monoObject, m_MonoField, value);
	}

	void ScriptField::GetDataRaw(void* result, GCHandle handle)
	{
		MonoObject* monoObject = GCManager::GetManagedObject(handle);
		if (monoObject == nullptr)
		{
			TR_ERROR("Couldnt find the object");
			return;
		}

		if (m_MonoField == nullptr) 
		{
			TR_ERROR("Mono field is null");
			return;
		}
		
		mono_field_get_value(monoObject, m_MonoField, result);
	}

	std::string ScriptField::GetDataStringRaw(GCHandle handle)
	{	
		MonoObject* monoObject = GCManager::GetManagedObject(handle);
		if (monoObject == nullptr)
		{
			TR_ERROR("Couldnt find the object");
			return "";
		}

		if (m_MonoField == nullptr)
		{
			TR_ERROR("Mono field is null");
			return "";
		}

		if (m_Type.TypeEnum != ScriptType::String) 
		{
			TR_ERROR("Can't get the string value of a non-string field");
			return "";
		}

		MonoString* monoStr = nullptr;
		mono_field_get_value(monoObject, m_MonoField, &monoStr);
		const std::string val = ScriptMarshal::MonoStringToUTF8(monoStr);
		return val;
	}

	void ScriptField::SetDataStringRaw(const char* value, GCHandle handle)
	{
		MonoObject* monoObject = GCManager::GetManagedObject(handle);
		if (monoObject == nullptr) 
		{
			TR_ERROR("Couldnt find the object");
			return;
		}

		if (m_MonoField == nullptr) 
		{
			TR_ERROR("Mono field is null");
			return;
		}

		if (m_Type.TypeEnum != ScriptType::String)
		{
			TR_ERROR("Can't set the string value of a non-string field");
			return;
		}
		
		MonoString* monoStr = ScriptMarshal::UTF8ToMonoString(value);
		mono_field_set_value(monoObject, m_MonoField, monoStr);
	}
	
	void ScriptField::SetDataVariantRaw(const Utils::Variant& value, GCHandle handle)
	{
		switch (m_Type.TypeEnum)
		{
		case ScriptType::Bool:
		{
			bool val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::Char:
		{
			char val = value;
			SetData<wchar_t>((wchar_t)val, handle);
			break;
		}
		case ScriptType::Int8:
		{
			int8_t val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::Int16:
		{
			int16_t val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::Int32:
		{
			int32_t val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::Int64:
		{
			int64_t val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::UInt8:
		{
			uint8_t val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::UInt16:
		{
			uint16_t val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::UInt32:
		{
			uint32_t val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::UInt64:
		{
			uint64_t val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::Float:
		{
			float val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::Double:
		{
			double val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::String:
		{
			const char*  val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::Vector2:
		{
			glm::vec2 val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::Vector3:
		{
			glm::vec3 val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::Color:
		{
			glm::vec4 val = value;
			SetData(val, handle);
			break;
		}
		case ScriptType::Entity:
		{
			UUID val = value;
			SetData(val, handle);
			break;
		}
		}
	}

	Utils::Variant ScriptField::GetDataVariantRaw(GCHandle handle)
	{
		switch (m_Type.TypeEnum)
		{
		case ScriptType::Bool:		return GetData<bool>(handle);
		case ScriptType::Char: 		return (char)GetData<wchar_t>(handle);
		case ScriptType::Int8: 		return GetData<int8_t>(handle);
		case ScriptType::Int16: 	return GetData<int16_t>(handle);
		case ScriptType::Int32: 	return GetData<int32_t>(handle);
		case ScriptType::Int64: 	return GetData<int64_t>(handle);
		case ScriptType::UInt8: 	return GetData<uint8_t>(handle);
		case ScriptType::UInt16: 	return GetData<uint16_t>(handle);
		case ScriptType::UInt32: 	return GetData<uint32_t>(handle);
		case ScriptType::UInt64: 	return GetData<uint64_t>(handle);
		case ScriptType::Float:		return GetData<float>(handle);
		case ScriptType::Double:	return GetData<double>(handle);
		case ScriptType::String:	return GetData<std::string>(handle);
		case ScriptType::Vector2:	return GetData<glm::vec2>(handle);
		case ScriptType::Vector3:	return GetData<glm::vec3>(handle);
		case ScriptType::Color:		return GetData<glm::vec4>(handle);
		case ScriptType::Entity:	return GetData<UUID>(handle);
		}

		return {};
	}

	void ScriptField::SetDataUUIDRaw(UUID value, GCHandle handle)
	{
		MonoObject* monoObject = GCManager::GetManagedObject(handle);
		if (monoObject == nullptr) 
		{
			TR_ERROR("Couldnt find the object");
			return;
		}

		if (m_MonoField == nullptr) 
		{
			TR_ERROR("Mono field is null");
			return;
		}

		if (m_Type.TypeEnum != ScriptType::Entity)
		{
			TR_ERROR("Field isn't of type 'Entity'");
			return;
		}

		const ScriptArray uuidArray = ScriptMarshal::UUIDToMonoArray(value);
		
		void* args[] = { uuidArray.GetMonoArray() };
		ScriptObject entityObj = ScriptObject::CreateInstace(*TR_API_CACHED_CLASS(Entity));
		ScriptMethod* constructor = ScriptCache::GetCachedMethod("Terran.Entity", ":.ctor(byte[])");
		constructor->Invoke(entityObj, args);
		
		mono_field_set_value(monoObject, m_MonoField, entityObj.GetMonoObject());
	}

	UUID ScriptField::GetDataUUIDRaw(GCHandle handle)
	{
		MonoObject* monoObject = GCManager::GetManagedObject(handle);
		if (monoObject == nullptr) 
		{
			TR_ERROR("Couldnt find the object");
			return {};
		}

		if (m_MonoField == nullptr) 
		{
			TR_ERROR("Mono field is null");
			return {};
		}

		if (m_Type.TypeEnum != ScriptType::Entity)
		{
			TR_ERROR("Field isn't of type 'Entity'");
			return {};
		}
		
		ScriptObject entityObj = mono_field_get_value_object(mono_domain_get(), m_MonoField, monoObject);
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

	ScriptArray ScriptField::GetArray(GCHandle handle)
	{
		MonoObject* monoObject = GCManager::GetManagedObject(handle);
		if (monoObject == nullptr) 
		{
			TR_ERROR("Couldnt find the object");
			return {};
		}
	
		if (m_MonoField == nullptr) 
		{
			TR_ERROR("Mono field is null");
			return {};
		}
	
		if (!m_Type.IsArray())
		{
			TR_ERROR("Field isn't an array type");
			return {};
		}
		
		MonoObject* arrayObj = mono_field_get_value_object(mono_domain_get(), m_MonoField, monoObject);

		if(!arrayObj)
		{
			ScriptArray defaultArray(m_Type.GetTypeClass()->GetMonoClass(), 1);
			mono_field_set_value(monoObject, m_MonoField, (MonoObject*)defaultArray.GetMonoArray());
			return defaultArray;
		}
		
		return ScriptArray::Create((MonoArray*)arrayObj);
	}

	void ScriptField::SetArray(ScriptArray array, GCHandle handle)
	{
		MonoObject* monoObject = GCManager::GetManagedObject(handle);
		if (monoObject == nullptr) 
		{
			TR_ERROR("Couldnt find the object");
			return;
		}
	
		if (m_MonoField == nullptr) 
		{
			TR_ERROR("Mono field is null");
			return;
		}
	
		if (!m_Type.IsArray())
		{
			TR_ERROR("Field isn't an array type");
			return;
		}
		
		mono_field_set_value(monoObject, m_MonoField, array.GetMonoArray());
	}
}
