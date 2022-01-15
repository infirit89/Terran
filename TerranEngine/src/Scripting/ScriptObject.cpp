#include "trpch.h"
#include "ScriptObject.h"

namespace TerranEngine 
{
	static ScriptFieldType GetFieldType(MonoType* monoType)
	{
		MonoTypeEnum type = (MonoTypeEnum)mono_type_get_type(monoType);

		switch (type)
		{
		case MONO_TYPE_BOOLEAN:		return ScriptFieldType::Bool;
		case MONO_TYPE_I:			return ScriptFieldType::Int;
		case MONO_TYPE_R4:			return ScriptFieldType::Float;
		case MONO_TYPE_R8:			return ScriptFieldType::Double;
		case MONO_TYPE_CHAR:		return ScriptFieldType::Char;
		}

		return ScriptFieldType::Unknown;
	}

	ScriptObject::ScriptObject(MonoObject* monoObject, std::unordered_map<uint32_t, Shared<ScriptMethod>>& methods)
		: m_MonoObject(monoObject), m_Methods(methods)
	{
		MonoClass* klass = mono_object_get_class(monoObject);

		// get the fields, and properties

		void* iter = nullptr;
		MonoClassField* field;

		std::hash<std::string> hasher;

		int test;

		while ((field = mono_class_get_fields(klass, &iter)) != nullptr) 
		{
			ScriptFieldType type = GetFieldType(mono_field_get_type(field));
			m_Fields[hasher(mono_field_get_name(field))] = CreateShared<ScriptField>(field, m_MonoObject, type);
		}
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

