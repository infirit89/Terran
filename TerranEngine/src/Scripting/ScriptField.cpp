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
		case MONO_TYPE_I4:			return ScriptFieldType::Int;
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
		m_Name = mono_field_get_name((MonoClassField*)m_MonoField);
		m_FieldType = ConvertFieldType(mono_field_get_type((MonoClassField*)m_MonoField));
		m_FieldVisibility = ConvertFieldVisibilty((MonoClassField*)m_MonoField);
	}

	void ScriptField::SetValue(void* value)
	{
		mono_field_set_value(mono_gchandle_get_target(m_MonoObjectGCHandle), (MonoClassField*)m_MonoField, value);
	}

	void ScriptField::GetValue(void* result)
	{
		mono_field_get_value(mono_gchandle_get_target(m_MonoObjectGCHandle), (MonoClassField*)m_MonoField, result);
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
