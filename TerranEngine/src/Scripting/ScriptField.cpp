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

	ScriptField::ScriptField(MonoClassField* monoField, MonoObject* monoObject)
		: m_MonoField(monoField), m_MonoObject(monoObject)
	{
		m_Name = mono_field_get_name(m_MonoField);
		m_FieldType = ConvertFieldType(mono_field_get_type(m_MonoField));
		m_FieldVisibility = ConvertFieldVisibilty(m_MonoField);
	}
}
