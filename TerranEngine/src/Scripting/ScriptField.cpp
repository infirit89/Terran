#include "trpch.h"
#include "ScriptField.h"

namespace TerranEngine 
{
	ScriptField::ScriptField(MonoClassField* monoField, MonoObject* monoObject)
		: m_MonoField(monoField), m_MonoObject(monoObject)
	{
	}
}

