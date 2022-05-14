#include "trpch.h"
#include "ScriptObject.h"
#include "GCManager.h"

#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/object.h>


namespace TerranEngine 
{
	ScriptObject::ScriptObject(GCHandle gcHandle)
		: m_GCHandle(gcHandle)
	{
		std::hash<std::string> hasher;

		MonoObject* monoObject = GCManager::GetMonoObject(m_GCHandle);

		MonoClass* klass = mono_object_get_class(monoObject);
		
		MonoClassField* field;
		void* iter = nullptr;

		while ((field = mono_class_get_fields(klass, &iter)) != nullptr) 
		{
			uint32_t hashedName = hasher(mono_field_get_name(field));
			ScriptField scriptField(field, monoObject);
			
			if (scriptField.GetVisibility() == ScriptFieldVisiblity::Public) 
			{
				m_FieldOrder.emplace_back(hashedName);
				m_PublicFields.emplace(hashedName, std::move(scriptField));
			}
		}
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
}

