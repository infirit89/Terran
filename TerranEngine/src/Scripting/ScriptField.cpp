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
			case ScriptType::LayerMask:
			{
				uint16_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			}
        }		

        template<>
        Utils::Variant ScriptFieldGetData<Utils::Variant>(GCHandle handle, ScriptField* field) 
        {
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
			case ScriptType::LayerMask: return ScriptFieldGetData<uint16_t>(handle, field);
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
