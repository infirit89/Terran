#include "trpch.h"
#include "ScriptField.h"
#include "ScriptMarshal.h"
#include "ScriptCache.h"
#include "ScriptArray.h"
#include "ManagedMethodThunks.h"
#include "ManagedObject.h"
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
        void ScriptFieldSetData<std::string>(std::string value, GCHandle handle, ScriptField* field)
        {
            ScriptUtils::SetFieldDataString(value.c_str(), handle, field->GetMonoField());
        }

        template<>
        void ScriptFieldSetData<Utils::Variant>(Utils::Variant value, GCHandle handle, ScriptField* field)
        {
			switch (field->GetType().GetNativeType())
			{
			case NativeType::Bool:
			{
				bool val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::Char:
			{
				char val = value;
				ScriptFieldSetData<wchar_t>((wchar_t)val, handle, field);
				break;
			}
			case NativeType::Int8:
			{
				int8_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::Int16:
			{
				int16_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::Int32:
			{
				int32_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::Int64:
			{
				int64_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::UInt8:
			{
				uint8_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::UInt16:
			{
				uint16_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::UInt32:
			{
				uint32_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::UInt64:
			{
				uint64_t val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::Float:
			{
				float val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::Double:
			{
				double val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::String:
			{
				const char*  val = value;
				ScriptFieldSetData<std::string>(val, handle, field);
				break;
			}
			case NativeType::Vector2:
			{
				glm::vec2 val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::Vector3:
			{
				glm::vec3 val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::Color:
			{
				glm::vec4 val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::Entity:
			{
				UUID val = value;
				ScriptFieldSetData(val, handle, field);
				break;
			}
			case NativeType::LayerMask:
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
			switch (field->GetType().GetNativeType())
			{
			case NativeType::Bool:		return ScriptFieldGetData<bool>(handle, field);
			case NativeType::Char: 		return (char)ScriptFieldGetData<wchar_t>(handle, field);
			case NativeType::Int8: 		return ScriptFieldGetData<int8_t>(handle, field);
			case NativeType::Int16: 	return ScriptFieldGetData<int16_t>(handle, field);
			case NativeType::Int32: 	return ScriptFieldGetData<int32_t>(handle, field);
			case NativeType::Int64: 	return ScriptFieldGetData<int64_t>(handle, field);
			case NativeType::UInt8: 	return ScriptFieldGetData<uint8_t>(handle, field);
			case NativeType::UInt16: 	return ScriptFieldGetData<uint16_t>(handle, field);
			case NativeType::UInt32: 	return ScriptFieldGetData<uint32_t>(handle, field);
			case NativeType::UInt64: 	return ScriptFieldGetData<uint64_t>(handle, field);
			case NativeType::Float:		return ScriptFieldGetData<float>(handle, field);
			case NativeType::Double:	return ScriptFieldGetData<double>(handle, field);
			case NativeType::String:	return ScriptFieldGetData<std::string>(handle, field);
			case NativeType::Vector2:	return ScriptFieldGetData<glm::vec2>(handle, field);
			case NativeType::Vector3:	return ScriptFieldGetData<glm::vec3>(handle, field);
			case NativeType::Color:		return ScriptFieldGetData<glm::vec4>(handle, field);
			case NativeType::Entity:	return ScriptFieldGetData<UUID>(handle, field);
			case NativeType::LayerMask: return ScriptFieldGetData<uint16_t>(handle, field);
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
		m_Type = ManagedType(mono_field_get_type(m_MonoField));

		uint32_t accessMask = mono_field_get_flags(m_MonoField) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;
		m_FieldVisibility = ConvertFieldVisibility(accessMask);
	}

	void ScriptField::CopyData(GCHandle sourceHandle, GCHandle destinationHandle)
	{
		if(m_Type.IsArray())
		{
			ScriptArray sourceArray = GetArray(sourceHandle);
			ScriptArray destinationArray = GetArray(destinationHandle);

			if(sourceArray.Length() != destinationArray.Length()) 
			{
				if (destinationArray)
					destinationArray.Resize(sourceArray.Length());
				else
					destinationArray = ScriptArray(sourceArray.GetElementType().GetTypeClass(), sourceArray.Length());
			}

			for (uint32_t i = 0; i < sourceArray.Length(); i++)
				destinationArray.Set(i, sourceArray[i]);

			SetArray(destinationArray, destinationHandle);

			return;
		}
			
		const auto& val = GetData<Utils::Variant>(sourceHandle);
		SetData<Utils::Variant>(val, destinationHandle);
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
			return ScriptArray();
		
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
