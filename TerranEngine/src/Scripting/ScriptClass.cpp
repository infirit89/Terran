#include "trpch.h"
#include "ScriptClass.h"

namespace TerranEngine
{
	ScriptClass::ScriptClass(MonoClass* monoClass)
		: m_MonoClass(monoClass)
	{
		MonoMethod* method;
		void* iter = nullptr;
		std::hash<std::string> hasher;
		while ((method = mono_class_get_methods(m_MonoClass, &iter)) != NULL) 
			m_Methods[hasher(mono_method_get_name(method))] = CreateShared<ScriptMethod>(method);
	}

	void ScriptClass::ExecuteStatic(const char* methodName)
	{

		std::hash<std::string> hasher;
		uint32_t hashedName = hasher(methodName);
		if (m_Methods.find(hashedName) != m_Methods.end())
			m_Methods[hashedName]->Execute();
		else
			TR_ERROR("No method found with the corresponding name");
	}
}

