#include "trpch.h"
#include "ScriptClass.h"

#include <mono/metadata/appdomain.h>

namespace TerranEngine
{
	ScriptClass::ScriptClass(MonoClass* monoClass)
		: m_MonoClass(monoClass)
	{
	}

	Shared<ScriptObject> ScriptClass::CreateInstance()
	{
		MonoObject* monoObject = mono_object_new(mono_domain_get(), m_MonoClass);
		mono_runtime_object_init(monoObject);

		uint32_t monoGCHandle = mono_gchandle_new(monoObject, true);
		
		return CreateShared<ScriptObject>(monoGCHandle);
	}

	Shared<ScriptMethod> ScriptClass::GetMethod(const char* methodSignature) 
	{
		MonoMethodDesc* monoDesc = mono_method_desc_new(methodSignature, false);
		if (!monoDesc)
		{
			TR_ERROR("Couldn't find a matching description ({0}) in the class {1}", methodSignature, mono_class_get_name(m_MonoClass));

			return NULL;
		}

		MonoMethod* monoMethod = mono_method_desc_search_in_class(monoDesc, m_MonoClass);

		if (!monoMethod)
		{
			TR_ERROR("Couldn't find the method with signature: {0} in class {1}", methodSignature, mono_class_get_name(m_MonoClass));
			
			return NULL;
		}

		Shared<ScriptMethod> method = CreateShared<ScriptMethod>(monoMethod);

		mono_method_desc_free(monoDesc);

		return method;
	}
}

