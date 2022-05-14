#include "trpch.h"
#include "ScriptClass.h"
#include "GCManager.h"

#include <mono/metadata/class.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>

namespace TerranEngine
{
	ScriptClass::ScriptClass(MonoClass* monoClass)
		: m_MonoClass(monoClass)
	{
		m_ClassName = mono_class_get_name(m_MonoClass);
		m_Namespace = mono_class_get_namespace(m_MonoClass);
	}

	ScriptObject ScriptClass::CreateInstance()
	{
		MonoObject* monoObject = mono_object_new(mono_domain_get(), m_MonoClass);
		mono_runtime_object_init(monoObject);

		return ScriptObject(GCManager::CreateStrongHadle(monoObject));
	}

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

	ScriptClass ScriptClass::GetParent()
	{
		MonoClass* monoClass = mono_class_get_parent(m_MonoClass);

		return ScriptClass(monoClass);
	}
}
