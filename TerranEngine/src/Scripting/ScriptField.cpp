#include "trpch.h"
#include "ScriptField.h"

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
		case MONO_TYPE_I4:			return ScriptFieldType::Int;
		case MONO_TYPE_I8:			return ScriptFieldType::Int64;

		// unsigned integer types
		case MONO_TYPE_U1:			return ScriptFieldType::UInt8;
		case MONO_TYPE_U2:			return ScriptFieldType::UInt16;
		case MONO_TYPE_U4:			return ScriptFieldType::UInt;
		case MONO_TYPE_U8:			return ScriptFieldType::UInt64;

		case MONO_TYPE_R4:			return ScriptFieldType::Float;
		case MONO_TYPE_R8:			return ScriptFieldType::Double;
		case MONO_TYPE_CHAR:		return ScriptFieldType::Char;
		case MONO_TYPE_STRING:		return ScriptFieldType::String;
		}

		return ScriptFieldType::None;
	}

	static ScriptFieldVisiblity ConvertFieldVisibility(uint32_t accessMask) 
	{
		switch (accessMask)
		{
		case MONO_FIELD_ATTR_PRIVATE:	return ScriptFieldVisiblity::Private;
		case MONO_FIELD_ATTR_PUBLIC:	return ScriptFieldVisiblity::Public;
		case MONO_FIELD_ATTR_FAMILY:	return ScriptFieldVisiblity::Protected;
		case MONO_FIELD_ATTR_ASSEMBLY:	return ScriptFieldVisiblity::Internal;
		}
	}

	ScriptField::ScriptField(void* monoField, uint32_t monoObjectGCHandle)
		: m_MonoField(monoField), m_MonoObjectGCHandle(monoObjectGCHandle)
	{
		memset(&m_CachedData, 0, sizeof(m_CachedData));

		m_Name = mono_field_get_name((MonoClassField*)m_MonoField);
		m_FieldType = ConvertFieldType(mono_field_get_type((MonoClassField*)m_MonoField));

		uint32_t accessMask = mono_field_get_flags((MonoClassField*)m_MonoField) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;
		m_FieldVisibility = ConvertFieldVisibility(accessMask);
	}

	template<typename T, typename CachedDataType>
	static void ExtractFieldData(void* result, CachedDataType& cachedData, MonoObject* monoObject, MonoClassField* monoField)
	{
		if (monoObject != nullptr && monoField != nullptr)
			cachedData = *(T*)result;

		*(T*)result = cachedData;
	}

	void ScriptField::SetValue(void* value)
	{
		MonoObject* monoObject = mono_gchandle_get_target(m_MonoObjectGCHandle);
		MonoClassField* monoField = (MonoClassField*)m_MonoField;

		if (monoObject == nullptr) 
			TR_ERROR("Couldn't set the value of field {0} because the object that it corresponds to is no longer valid", m_Name);

		if (monoField == nullptr) 
			TR_ERROR("Couldn't set the value of field {0} because it wasn't found", m_Name);
		
		if (monoObject != nullptr && monoField != nullptr)
			mono_field_set_value(mono_gchandle_get_target(m_MonoObjectGCHandle), (MonoClassField*)m_MonoField, value);

		// bad bad bad bad bad bad bad bad bad bad bad bad
		switch (m_FieldType)
		{
		case TerranEngine::ScriptFieldType::Bool:
		{
			ExtractFieldData<bool>(value, m_CachedData.bValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::Int8:
		{
			ExtractFieldData<int8_t>(value, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::Int16:
		{
			ExtractFieldData<int16_t>(value, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::Int:
		{
			ExtractFieldData<int32_t>(value, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::Int64:
		{
			ExtractFieldData<int64_t>(value, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::UInt8:
		{
			ExtractFieldData<uint8_t>(value, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::UInt16:
		{
			ExtractFieldData<uint16_t>(value, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::UInt:
		{
			ExtractFieldData<uint32_t>(value, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::UInt64:
		{
			ExtractFieldData<uint64_t>(value, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::Float:
		{
			ExtractFieldData<float>(value, m_CachedData.dValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::Double:
		{
			ExtractFieldData<double>(value, m_CachedData.dValue, monoObject, monoField);
			break;
		}
		}
	}

	void ScriptField::GetValue(void* result)
	{
		MonoObject* monoObject = mono_gchandle_get_target(m_MonoObjectGCHandle);
		MonoClassField* monoField = (MonoClassField*)m_MonoField;

		if (monoObject == nullptr)
			TR_ERROR("Couldnt find the object");


		if (monoField == nullptr)
			TR_ERROR("Mono field is null");
		
		if (monoObject != nullptr && monoField != nullptr)
			mono_field_get_value(monoObject, monoField, result);
		
		// very terrible
		switch (m_FieldType)
		{
		// ---- Boolean ----
		case TerranEngine::ScriptFieldType::Bool: 
		{
			ExtractFieldData<bool>(result, m_CachedData.bValue, monoObject, monoField);
			break;
		}
		// -----------------
		
		// ---- Signed integers ----
		case TerranEngine::ScriptFieldType::Int8:
		{
			ExtractFieldData<int8_t>(result, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::Int16:
		{
			ExtractFieldData<int16_t>(result, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::Int:
		{
			ExtractFieldData<int32_t>(result, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::Int64:
		{
			ExtractFieldData<int64_t>(result, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		// -------------------------
		
		// ---- Unsigned integers ----
		case TerranEngine::ScriptFieldType::UInt8:
		{
			ExtractFieldData<uint8_t>(result, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::UInt16:
		{
			ExtractFieldData<uint16_t>(result, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::UInt:
		{
			ExtractFieldData<uint32_t>(result, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::UInt64:
		{
			ExtractFieldData<uint64_t>(result, m_CachedData.iValue, monoObject, monoField);
			break;
		}
		// ---------------------------
		
		// ---- Floating point ----
		case TerranEngine::ScriptFieldType::Float:
		{
			ExtractFieldData<float>(result, m_CachedData.dValue, monoObject, monoField);
			break;
		}
		case TerranEngine::ScriptFieldType::Double:
		{
			ExtractFieldData<double>(result, m_CachedData.dValue, monoObject, monoField);
			break;
		}
		// ------------------------
		}
	}

	const char* ScriptField::GetValue()
	{
		if (m_FieldType != ScriptFieldType::String) 
		{
			TR_ERROR("Can't get the string value of a non-string field");
			return "";
		}

		MonoString* string = nullptr;

		mono_field_get_value(mono_gchandle_get_target(m_MonoObjectGCHandle), (MonoClassField*)m_MonoField, &string);

		return ScriptString(string).GetUTF8Str();
	}

	void ScriptField::SetValue(const char* value)
	{
		if (m_FieldType != ScriptFieldType::String)
		{
			TR_ERROR("Can't set the string value of a non-string field");
			return;
		}

		ScriptString string((const char*)value);

		mono_field_set_value(mono_gchandle_get_target(m_MonoObjectGCHandle), (MonoClassField*)m_MonoField, string.GetStringInternal());
	}
}
