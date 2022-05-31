#include "trpch.h"
#include "ScriptObject.h"
#include "GCManager.h"

#include "Core/Hash.h"

#include <mono/metadata/attrdefs.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>

namespace TerranEngine 
{
	ScriptObject::ScriptObject(MonoObject* monoObject)
		: m_MonoObject(monoObject)
	{
		MonoClass* klass = mono_object_get_class(monoObject);

		MonoClassField* field;
		void* iter = nullptr;

		while ((field = mono_class_get_fields(klass, &iter)) != nullptr)
		{
			uint32_t hashedName = Hash::FNVHash(mono_field_get_name(field));
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
		uint32_t hashedName = Hash::FNVHash(fieldName);

		if (m_PublicFields.find(hashedName) != m_PublicFields.end())
			return m_PublicFields[hashedName];
		else 
			TR_WARN("No field with the corresponding name");

		return { };
	}

	ScriptObject ScriptObject::CreateInstace(ScriptClass klass)
	{
		MonoObject* monoObject = mono_object_new(mono_domain_get(), klass.GetMonoClass());
		mono_runtime_object_init(monoObject);
		
		return monoObject;
	}
}
