#include "trpch.h"
#include "ScriptObject.h"

namespace TerranEngine 
{
	ScriptObject::ScriptObject(MonoObject* monoObject, std::unordered_map<uint32_t, Shared<ScriptMethod>>& methods)
		: m_MonoObject(monoObject), m_Methods(methods)
	{
		MonoClass* klass = mono_object_get_class(monoObject);

		void* iter = nullptr;
		MonoClassField* field;

		std::hash<std::string> hasher;

		while ((field = mono_class_get_fields(klass, &iter)) != nullptr) 
			m_Fields[hasher(mono_field_get_name(field))] = CreateShared<ScriptField>(field, m_MonoObject);
	}

	void ScriptObject::Execute(const char* methodName, ScriptMethodParameterList parameterList)
	{
		std::hash<std::string> hasher;
		uint32_t hashedName = hasher(methodName);

		if (m_Methods.find(hashedName) != m_Methods.end())
			m_Methods[hashedName]->Execute(this, parameterList);
		else
			TR_ERROR("No method found with the corresponding name");
	}

	Shared<ScriptField> ScriptObject::GetField(const char* fieldName)
	{
		std::hash<std::string> hasher;
		uint32_t hashedName = hasher(fieldName);

		if (m_Fields.find(hashedName) != m_Fields.end())
			return m_Fields[hashedName];
		else
			TR_ERROR("No field with the corresponding name");
	}
}

