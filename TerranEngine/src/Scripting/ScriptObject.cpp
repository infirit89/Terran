#include "trpch.h"
#include "ScriptObject.h"

#include <mono/metadata/attrdefs.h>

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

		MonoClassField* testField = mono_class_get_field_from_name(klass, "Test");

		if (testField != nullptr)
			TR_TRACE("found");

		while ((field = mono_class_get_fields(klass, &iter)) != nullptr) 
		{
			uint32_t fieldVisibilty = mono_field_get_flags(field) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

			if (fieldVisibilty == MONO_FIELD_ATTR_PUBLIC) 
			{
				uint32_t hashedName = hasher(mono_field_get_name(field));
				ScriptField scriptField(field, m_MonoGCHandle);
				
				TR_TRACE(mono_field_get_name(field));

				m_FieldOrder.emplace_back(hashedName);
				m_PublicFields.emplace(hashedName, std::move(scriptField));
			}

		}
	}

	ScriptObject::~ScriptObject()
	{
	}

	void ScriptObject::Uninitialize()
	{
		TR_INFO("Mono object released");
		mono_gchandle_free(m_MonoGCHandle);
		m_MonoGCHandle = 0;
	}

	ScriptField ScriptObject::GetPublicField(const char* fieldName)
	{
		std::hash<std::string> hasher;
		uint32_t hashedName = hasher(fieldName);

		if (m_PublicFields.find(hashedName) != m_PublicFields.end())
			return m_PublicFields[hashedName];
		else 
			TR_WARN("No field with the corresponding name");

		return { };
	}

	void* ScriptObject::GetNativeObject() const
	{
		MonoObject* obj = mono_gchandle_get_target(m_MonoGCHandle);

		if (obj == NULL) 
			TR_ERROR("Script object is null");

		return (void*)obj;
	}
}

