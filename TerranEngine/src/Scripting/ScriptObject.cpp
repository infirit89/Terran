#include "trpch.h"
#include "ScriptObject.h"
#include "ScriptMethod.h"

namespace TerranEngine 
{
	ScriptObject::ScriptObject(MonoObject* monoObject, std::unordered_map<uint32_t, Shared<ScriptMethod>>& methods)
		: m_MonoObject(monoObject), m_Methods(methods)
	{
	}

	void ScriptObject::Execute(const char* methodName)
	{
		std::hash<std::string> hasher;
		uint32_t hashedName = hasher(methodName);

		if (m_Methods.find(hashedName) != m_Methods.end())
			//m_Methods[hashedName]->Execute(this);
			TR_WARN("fuck you, not implemented");
		else
			TR_ERROR("No method found with the corresponding name");
	}
}

