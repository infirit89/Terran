#include "trpch.h"
#include "ScriptClass.h"

#include <mono/metadata/appdomain.h>

namespace TerranEngine
{
	ScriptClass::ScriptClass(MonoClass* monoClass)
		: m_MonoClass(monoClass)
	{
		std::hash<std::string> hasher;

		// NOTE: maybe change it to use method description
		while (monoClass != NULL)
		{
			MonoMethod* method;
			void* iter = nullptr;
			while ((method = mono_class_get_methods(monoClass, &iter)) != NULL)
			{
				uint32_t hashedName = hasher(mono_method_get_name(method));
				if(m_Methods.find(hashedName) == m_Methods.end())
					m_Methods[hashedName] = CreateShared<ScriptMethod>(method);
			}

			monoClass = mono_class_get_parent(monoClass);
		}
	}

	Shared<ScriptObject> ScriptClass::CreateInstance()
	{
		MonoObject* monoObject = mono_object_new(mono_domain_get(), m_MonoClass);
		mono_runtime_object_init(monoObject);

		return CreateShared<ScriptObject>(monoObject, m_Methods);
	}

	void ScriptClass::ExecuteStatic(const char* methodName, ScriptMethodParameterList parameterList)
	{

		std::hash<std::string> hasher;
		uint32_t hashedName = hasher(methodName);
		if (m_Methods.find(hashedName) != m_Methods.end())
			m_Methods[hashedName]->Execute(nullptr, parameterList);
		else
			TR_ERROR("No method found with the corresponding name");
	}
}

