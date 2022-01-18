#include "trpch.h"
#include "ScriptField.h"

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

	ScriptField::ScriptField(MonoClassField* monoField, MonoObject* monoObject)
		: m_MonoField(monoField), m_MonoObject(monoObject)
	{
		m_Name = mono_field_get_name(m_MonoField);
		m_FieldType = ConvertFieldType(mono_field_get_type(m_MonoField));
	}
}
