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
		case MONO_TYPE_U2:			return ScriptFieldType::Uint16;
		case MONO_TYPE_U4:			return ScriptFieldType::UInt;
		case MONO_TYPE_U8:			return ScriptFieldType::UInt64;

		case MONO_TYPE_R4:			return ScriptFieldType::Float;
		case MONO_TYPE_R8:			return ScriptFieldType::Double;
		case MONO_TYPE_CHAR:		return ScriptFieldType::Char;
		case MONO_TYPE_STRING:		return ScriptFieldType::String;
		}

		return ScriptFieldType::Unknown;
	}

	static ScirptFieldVisibility ConvertFieldVisibilty(MonoClassField* monofield) 
	{
		uint32_t visibility = mono_field_get_flags(monofield) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;
		
		switch (visibility)
		{
		case MONO_FIELD_ATTR_PRIVATE:	return ScirptFieldVisibility::Private;
		case MONO_FIELD_ATTR_FAMILY:	return ScirptFieldVisibility::Protected;
		case MONO_FIELD_ATTR_ASSEMBLY:	return ScirptFieldVisibility::Internal;
		case MONO_FIELD_ATTR_PUBLIC:	return ScirptFieldVisibility::Public;
		}

		return ScirptFieldVisibility::None;
	}

	ScriptField::ScriptField(void* monoField, uint32_t monoObjectGCHandle)
		: m_MonoField(monoField), m_MonoObjectGCHandle(monoObjectGCHandle)
	{
		memset(&m_CachedData, 0, sizeof(m_CachedData));

		m_Name = mono_field_get_name((MonoClassField*)m_MonoField);
		m_FieldType = ConvertFieldType(mono_field_get_type((MonoClassField*)m_MonoField));
		m_FieldVisibility = ConvertFieldVisibilty((MonoClassField*)m_MonoField);
	}

	void ScriptField::SetValue(void* value)
	{
		MonoObject* monoObject = mono_gchandle_get_target(m_MonoObjectGCHandle);
		MonoClassField* monoField = (MonoClassField*)m_MonoField;

		if (monoObject == nullptr) 
		{
			TR_ERROR("Couldn't set the value of field {0} because the object that it corresponds to is no longer valid", m_Name);
			return;
		}

		if (monoField == nullptr) 
		{
			TR_ERROR("Couldn't set the value of field {0} because it wasn't found", m_Name);
			return;
		}

		mono_field_set_value(mono_gchandle_get_target(m_MonoObjectGCHandle), (MonoClassField*)m_MonoField, value);
	}

	void ScriptField::GetValue(void* result)
	{
		MonoObject* monoObject = mono_gchandle_get_target(m_MonoObjectGCHandle);
		MonoClassField* monoField = (MonoClassField*)m_MonoField;

		if (monoObject == nullptr)
			TR_ERROR("Couldnt find the object");


		if (monoField == nullptr)
			TR_ERROR("Mono field is null");
		
		switch (m_FieldType)
		{
		case TerranEngine::ScriptFieldType::Bool: 
		{
			if (monoObject != nullptr && monoField != nullptr) 
			{
				mono_field_get_value(monoObject, monoField, result);
				m_CachedData.bValue = *(bool*)result;
			}

			*(bool*)result = m_CachedData.bValue;
			break;
		}
		case TerranEngine::ScriptFieldType::Int:
		{
			if (monoObject != nullptr && monoField != nullptr)
			{
				mono_field_get_value(monoObject, monoField, result);
				m_CachedData.iValue = *(int*)result;
			}

			*(int*)result = m_CachedData.iValue;
			break;
		}
		case TerranEngine::ScriptFieldType::Float:
		{
			if (monoObject != nullptr && monoField != nullptr)
			{
				mono_field_get_value(monoObject, monoField, result);
				m_CachedData.dValue = *(float*)result;
			}

			*(float*)result = m_CachedData.dValue;
			break;
		}
		case TerranEngine::ScriptFieldType::Double:
		{
			if (monoObject != nullptr && monoField != nullptr)
			{
				mono_field_get_value(monoObject, monoField, result);
				m_CachedData.dValue = *(double*)result;
			}

			*(double*)result = m_CachedData.dValue;
			break;
		}
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
