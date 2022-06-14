#include "trpch.h"
#include "ScriptField.h"
#include "ScriptMarshal.h"

#include <mono/metadata/attrdefs.h>

namespace TerranEngine 
{
	static ScriptFieldType ConvertFieldType(MonoType* monoType)
	{
		MonoTypeEnum type = (MonoTypeEnum)mono_type_get_type(monoType);

		switch (type)
		{
		case MONO_TYPE_BOOLEAN:		return ScriptFieldType::Bool;

		// signed integer types
		case MONO_TYPE_I1:			return ScriptFieldType::Int8;
		case MONO_TYPE_I2:			return ScriptFieldType::Int16;
		case MONO_TYPE_I4:			return ScriptFieldType::Int32;
		case MONO_TYPE_I8:			return ScriptFieldType::Int64;

		// unsigned integer types
		case MONO_TYPE_U1:			return ScriptFieldType::UInt8;
		case MONO_TYPE_U2:			return ScriptFieldType::UInt16;
		case MONO_TYPE_U4:			return ScriptFieldType::UInt64;
		case MONO_TYPE_U8:			return ScriptFieldType::UInt64;

		case MONO_TYPE_R4:			return ScriptFieldType::Float;
		case MONO_TYPE_R8:			return ScriptFieldType::Double;

		case MONO_TYPE_CHAR:		return ScriptFieldType::Char;

		case MONO_TYPE_STRING:		return ScriptFieldType::String;
		case MONO_TYPE_VALUETYPE: 
		{
			MonoClass* typeClass = mono_class_from_mono_type(monoType);
			const char* typeClassTypeName = mono_class_get_name(typeClass);
			const char* typeClassNamespace = mono_class_get_namespace(typeClass);

			// NOTE: string comparing is slow, think of a better way to do this
			if (strcmp(typeClassNamespace, "TerranScriptCore") == 0) 
			{
				if (strcmp(typeClassTypeName, "Vector2") == 0)
					return ScriptFieldType::Vector2;
				if (strcmp(typeClassTypeName, "Vector3") == 0)
					return ScriptFieldType::Vector3;
			}
		}
		}

		return ScriptFieldType::None;
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
		m_FieldType = ConvertFieldType(mono_field_get_type(m_MonoField));

		uint32_t accessMask = mono_field_get_flags(m_MonoField) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;
		m_FieldVisibility = ConvertFieldVisibility(accessMask);
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

	const char* ScriptField::GetDataStringRaw(GCHandle handle)
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

		if (m_FieldType != ScriptFieldType::String) 
		{
			TR_ERROR("Can't get the string value of a non-string field");
			return "";
		}

		MonoString* monoStr = nullptr;
		mono_field_get_value(monoObject, m_MonoField, &monoStr);
		return ScriptMarshal::MonoStringToUTF8(monoStr).c_str();
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

		if (m_FieldType != ScriptFieldType::String)
		{
			TR_ERROR("Can't set the string value of a non-string field");
			return;
		}
		
		MonoString* monoStr = ScriptMarshal::UTF8ToMonoString(value);
		mono_field_set_value(monoObject, m_MonoField, monoStr);
	}

	void ScriptField::SetDataVariantRaw(const Utils::Variant& variant, GCHandle handle)
	{
		switch (m_FieldType)
		{
		case ScriptFieldType::Bool:
		{
			bool val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::Char:
		{
			char val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::Int8:
		{
			int8_t val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::Int16:
		{
			int16_t val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::Int32:
		{
			int32_t val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::Int64:
		{
			int64_t val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::UInt8:
		{
			uint8_t val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::UInt16:
		{
			uint16_t val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::UInt32:
		{
			uint32_t val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::UInt64:
		{
			uint64_t val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::Float:
		{
			float val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::Double:
		{
			double val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::String:
		{
			const char*  val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::Vector2:
		{
			glm::vec2 val = variant;
			SetData(val, handle);
			break;
		}
		case ScriptFieldType::Vector3:
		{
			glm::vec3 val = variant;
			SetData(val, handle);
			break;
		}
		}
	}

	Utils::Variant ScriptField::GetDataVariantRaw(GCHandle handle)
	{
		switch (m_FieldType)
		{
		case ScriptFieldType::Bool:	return GetData<bool>(handle);
		case ScriptFieldType::Char: return GetData<char>(handle);
		case ScriptFieldType::Int8: return GetData<int8_t>(handle);
		case ScriptFieldType::Int16: return GetData<int16_t>(handle);
		case ScriptFieldType::Int32: return GetData<int32_t>(handle);
		case ScriptFieldType::Int64: return GetData<int64_t>(handle);
		case ScriptFieldType::UInt8: return GetData<uint8_t>(handle);
		case ScriptFieldType::UInt16: return GetData<uint16_t>(handle);
		case ScriptFieldType::UInt32: return GetData<uint32_t>(handle);
		case ScriptFieldType::UInt64: return GetData<uint64_t>(handle);
		case ScriptFieldType::Float: return GetData<float>(handle);
		case ScriptFieldType::Double: return GetData<double>(handle);
		case ScriptFieldType::String: return GetData<const char*>(handle);
		case ScriptFieldType::Vector2: return GetData<glm::vec2>(handle);
		case ScriptFieldType::Vector3: return GetData<glm::vec3>(handle);
		}

		return {};
	}
}
