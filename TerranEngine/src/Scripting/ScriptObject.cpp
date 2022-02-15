#include "trpch.h"
#include "ScriptObject.h"

namespace TerranEngine 
{
	ScriptObject::ScriptObject(uint32_t monoGCHandle)
		: m_MonoGCHandle(monoGCHandle)
	{
		std::hash<std::string> hasher;

		MonoObject* monoObject = (MonoObject*)GetNativeObject();

		MonoClass* klass = mono_object_get_class(monoObject);
		
		MonoClassField* field;
		void* iter = nullptr;

		while ((field = mono_class_get_fields(klass, &iter)) != nullptr) 
			m_Fields[hasher(mono_field_get_name(field))] = CreateShared<ScriptField>(field, monoObject);
	}

	ScriptObject::~ScriptObject()
	{
	}

	Shared<ScriptField> ScriptObject::GetField(const char* fieldName)
	{
		std::hash<std::string> hasher;
		uint32_t hashedName = hasher(fieldName);

		if (m_Fields.find(hashedName) != m_Fields.end())
			return m_Fields[hashedName];
		else 
			TR_ERROR("No field with the corresponding name");

		return nullptr;
	}

	std::vector<Shared<ScriptField>> ScriptObject::GetFields()
	{
		std::vector<Shared<ScriptField>> fields;

		fields.reserve(m_Fields.size());
		for (auto it = m_Fields.begin(); it != m_Fields.end(); it++)
			fields.push_back(it->second);

		return fields;
	}

	std::vector<Shared<ScriptField>> ScriptObject::GetPublicFields()
	{
		std::vector<Shared<ScriptField>> fields;

		fields.reserve(m_Fields.size());
		for (auto it = m_Fields.begin(); it != m_Fields.end(); it++) 
		{
			if(it->second->GetVisibility() == ScirptFieldVisibility::Public)
				fields.push_back(it->second);
		}

		return fields;
	}

	inline void* ScriptObject::GetNativeObject() const
	{
		MonoObject* obj = mono_gchandle_get_target(m_MonoGCHandle);

		if (obj == NULL) TR_ERROR("Script object is null");

		return obj;
	}
}

