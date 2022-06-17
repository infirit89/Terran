#include "trpch.h"
#include "ScriptObject.h"
#include "ScriptClass.h"

#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>

namespace TerranEngine 
{
	ScriptObject::ScriptObject(MonoObject* monoObject)
		: m_MonoObject(monoObject)
	{ }
	
	ScriptObject ScriptObject::CreateInstace(const ScriptClass& klass)
	{
		MonoObject* monoObject = mono_object_new(mono_domain_get(), klass.GetMonoClass());
		mono_runtime_object_init(monoObject);
		
		return monoObject;
	}

	ScriptClass ScriptObject::GetClass()
	{
		if(!m_MonoObject)
			return {};
			
		MonoClass* monoClass = mono_object_get_class(m_MonoObject);

		return { monoClass };
	}
}
