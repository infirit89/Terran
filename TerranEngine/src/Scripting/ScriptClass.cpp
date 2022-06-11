#include "trpch.h"
#include "ScriptClass.h"
#include "GCManager.h"

#include <mono/metadata/class.h>
#include <mono/metadata/debug-helpers.h>

namespace TerranEngine
{
	ScriptClass::ScriptClass(MonoClass* monoClass)
		: m_MonoClass(monoClass)
	{ }

	ScriptMethod ScriptClass::GetMethod(const char* methodSignature) 
	{
		MonoMethodDesc* monoDesc = mono_method_desc_new(methodSignature, false);
		if (!monoDesc)
		{
			TR_WARN("Couldn't create the method description with method signature: {0}", methodSignature);
			return NULL;
		}

		MonoMethod* monoMethod = mono_method_desc_search_in_class(monoDesc, m_MonoClass);

		if (!monoMethod)
		{
			TR_WARN("Couldn't find the method with signature: {0} in class {1}", methodSignature, mono_class_get_name(m_MonoClass));
			return NULL;
		}

		mono_method_desc_free(monoDesc);

		return ScriptMethod(monoMethod);
	}

	ScriptField ScriptClass::GetFieldFromToken(uint32_t fieldToken)
	{
		ScriptField field;
		MonoClassField* monoField = mono_class_get_field(m_MonoClass, fieldToken);

		if(monoField)
			field = ScriptField(monoField);
		else
			TR_ERROR("Couldn't find the field with field token: {0} in class: {1}", fieldToken, m_ClassName);
		
		return field;
	}

	ScriptClass ScriptClass::GetParent()
	{
		MonoClass* monoClass = mono_class_get_parent(m_MonoClass);

		return ScriptClass(monoClass);
	}

	bool ScriptClass::IsInstanceOf(ScriptClass* parent, bool checkInterfaces)
	{
		return mono_class_is_assignable_from(m_MonoClass, parent->m_MonoClass);
	}
}
