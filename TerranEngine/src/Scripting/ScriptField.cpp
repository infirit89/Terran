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
	namespace detail 
	{
        template<>
        std::string ScriptFieldGetData<std::string>(GCHandle handle, ScriptField* field) 
        {
            std::string value = ScriptUtils::GetFieldDataString(handle, field->GetMonoField());
            return value;
        }

        template<>
        void ScriptFieldSetData<const char*>(const char* value, GCHandle handle, ScriptField* field) 
        {
            ScriptUtils::SetFieldDataString(value, handle, field->GetMonoField());
        }

        template<>
        void ScriptFieldSetData<Utils::Variant>(Utils::Variant value, GCHandle handle, ScriptField* field)
        {
            // SetDataVariantRaw(value, handle);
			switch (field->GetType().TypeEnum)
			{
			case ScriptType::Bool:
			{
				bool val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::Char:
			{
				char val = value;
				ScriptFieldSetData<wchar_t>((wchar_t)val, handle, field);
				break;
			}
			case ScriptType::Int8:
			{
				int8_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::Int16:
			{
				int16_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::Int32:
			{
				int32_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::Int64:
			{
				int64_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::UInt8:
			{
				uint8_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::UInt16:
			{
				uint16_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::UInt32:
			{
				uint32_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::UInt64:
			{
				uint64_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::Float:
			{
				float val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::Double:
			{
				double val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::String:
			{
				const char*  val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::Vector2:
			{
				glm::vec2 val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::Vector3:
			{
				glm::vec3 val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::Color:
			{
				glm::vec4 val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case ScriptType::Entity:
			{
				UUID val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			}
        }		

        template<>
        Utils::Variant ScriptFieldGetData<Utils::Variant>(GCHandle handle, ScriptField* field) 
        {
            //Utils::Variant value = GetDataVariantRaw(handle);
            //return value;

			switch (field->GetType().TypeEnum)
			{
			case ScriptType::Bool:		return ScriptFieldGetData<bool>(handle, field);
			case ScriptType::Char: 		return (char)ScriptFieldGetData<wchar_t>(handle, field);
			case ScriptType::Int8: 		return ScriptFieldGetData<int8_t>(handle, field);
			case ScriptType::Int16: 	return ScriptFieldGetData<int16_t>(handle, field);
			case ScriptType::Int32: 	return ScriptFieldGetData<int32_t>(handle, field);
			case ScriptType::Int64: 	return ScriptFieldGetData<int64_t>(handle, field);
			case ScriptType::UInt8: 	return ScriptFieldGetData<uint8_t>(handle, field);
			case ScriptType::UInt16: 	return ScriptFieldGetData<uint16_t>(handle, field);
			case ScriptType::UInt32: 	return ScriptFieldGetData<uint32_t>(handle, field);
			case ScriptType::UInt64: 	return ScriptFieldGetData<uint64_t>(handle, field);
			case ScriptType::Float:		return ScriptFieldGetData<float>(handle, field);
			case ScriptType::Double:	return ScriptFieldGetData<double>(handle, field);
			case ScriptType::String:	return ScriptFieldGetData<std::string>(handle, field);
			case ScriptType::Vector2:	return ScriptFieldGetData<glm::vec2>(handle, field);
			case ScriptType::Vector3:	return ScriptFieldGetData<glm::vec3>(handle, field);
			case ScriptType::Color:		return ScriptFieldGetData<glm::vec4>(handle, field);
			case ScriptType::Entity:	return ScriptFieldGetData<UUID>(handle, field);
			}

			return {};
        }

        template<>
        void ScriptFieldSetData<UUID>(UUID value, GCHandle handle, ScriptField* field)
        {
            ScriptUtils::SetFieldDataUUID(value, handle, field->GetMonoField());
        }		

        template<>
        UUID ScriptFieldGetData<UUID>(GCHandle handle, ScriptField* field) 
        {
            UUID value = ScriptUtils::GetFieldDataUUID(handle, field->GetMonoField());
            return value;
        }
	}

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

	bool ScriptField::IsStatic() const { return mono_field_get_flags(m_MonoField) & MONO_FIELD_ATTR_STATIC; }

	std::string ScriptField::GetDataStringRaw(GCHandle handle)
	{	
		if (m_Type.TypeEnum != ScriptType::String) 
		{
			TR_ERROR("Can't get the string value of a non-string field");
			return "";
		}

		MonoString* monoStr = nullptr;
		ScriptUtils::GetFieldDataRaw(&monoStr, m_MonoField, handle);
		const std::string val = ScriptMarshal::MonoStringToUTF8(monoStr);
		return val;
	}

	void ScriptField::SetDataStringRaw(const char* value, GCHandle handle)
	{
		if (m_Type.TypeEnum != ScriptType::String)
		{
			TR_ERROR("Can't set the string value of a non-string field");
			return;
		}
		
		MonoString* monoStr = ScriptMarshal::UTF8ToMonoString(value);
		ScriptUtils::SetFieldDataRaw(monoStr, m_MonoField, handle);
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
		
		ScriptUtils::SetFieldDataRaw(entityObj.GetMonoObject(), m_MonoField, handle);
	}

	UUID ScriptField::GetDataUUIDRaw(GCHandle handle)
	{
		if (m_Type.TypeEnum != ScriptType::Entity)
		{
			TR_ERROR("Field isn't of type 'Entity'");
			return {};
		}
		
		ScriptObject entityObj = ScriptUtils::GetFieldValueObject(m_MonoField, handle);
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
		if (!m_Type.IsArray())
		{
			TR_ERROR("Field isn't an array type");
			return {};
		}
		
		MonoObject* arrayObj = ScriptUtils::GetFieldValueObject(m_MonoField, handle);

		if(!arrayObj)
		{
			ScriptArray defaultArray(m_Type.GetTypeClass()->GetMonoClass(), 1);
			ScriptUtils::SetFieldDataRaw(defaultArray.GetMonoArray(), m_MonoField, handle);
			return defaultArray;
		}
		
		return ScriptArray::Create((MonoArray*)arrayObj);
	}

	void ScriptField::SetArray(ScriptArray array, GCHandle handle)
	{
		if (!m_Type.IsArray())
		{
			TR_ERROR("Field isn't an array type");
			return;
		}
		
		ScriptUtils::SetFieldDataRaw(array.GetMonoArray(), m_MonoField, handle);
	}
}
