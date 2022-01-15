#include "trpch.h"
#include "ScriptField.h"

namespace TerranEngine 
{
	ScriptField::ScriptField(MonoClassField* monoField, MonoObject* monoObject, ScriptFieldType& fieldType)
		: m_MonoField(monoField), m_MonoObject(monoObject), m_FieldType(fieldType)
	{
	}
}

