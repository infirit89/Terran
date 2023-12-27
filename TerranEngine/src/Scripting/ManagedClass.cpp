#include "trpch.h"
#include "ManagedClass.h"
#include "GCManager.h"
#include "ManagedMetadata.h"

#include <mono/metadata/class.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>

namespace TerranEngine
{
	ManagedClass::ManagedClass(MonoClass* monoClass)
		: m_MonoClass(monoClass)
	{
		m_ClassName = mono_class_get_name(m_MonoClass);
		m_Namespace = mono_class_get_namespace(m_MonoClass);
	}

	ManagedMethod ManagedClass::GetMethod(const char* methodSignature) const
	{
		MonoMethodDesc* monoDesc = mono_method_desc_new(methodSignature, false);
		if (!monoDesc)
		{
			TR_WARN("Couldn't create the method description with method signature: {0}", methodSignature);
			return NULL;
		}

		MonoMethod* monoMethod = mono_method_desc_search_in_class(monoDesc, m_MonoClass);

		if (!monoMethod)
		{
			TR_WARN("Couldn't find the method with signature: {0} in class {1}", methodSignature, mono_class_get_name(m_MonoClass));
			return NULL;
		}

		mono_method_desc_free(monoDesc);

		return ManagedMethod(monoMethod);
	}

	ScriptField ManagedClass::GetFieldFromToken(uint32_t fieldToken) const
	{
		ScriptField field;
		MonoClassField* monoField = mono_class_get_field(m_MonoClass, fieldToken);

		if(monoField)
			field = ScriptField(monoField);
		else
			TR_ERROR("Couldn't find the field with field token: {0} in class: {1}", fieldToken, m_ClassName);
		
		return field;
	}

	ScriptField ManagedClass::GetFieldFromName(const std::string& fieldName) const
	{
		ScriptField field;
		MonoClassField* monoField = mono_class_get_field_from_name(m_MonoClass, fieldName.c_str());

		if(monoField)
			field = ScriptField(monoField);
		else
			TR_ERROR("Couldn't find the field with name: {0} in class: {1}", fieldName, m_ClassName);

		return field;
	}

	ManagedClass ManagedClass::GetParent()
	{
		MonoClass* monoClass = mono_class_get_parent(m_MonoClass);

		return ManagedClass(monoClass);
	}

	bool ManagedClass::IsInstanceOf(ManagedClass* parent)
	{
		return mono_class_is_assignable_from(m_MonoClass, parent->m_MonoClass);
	}

	bool ManagedClass::IsSubclassOf(ManagedClass* parent, bool checkInterfaces)
	{
		return mono_class_is_subclass_of(m_MonoClass, parent->m_MonoClass, checkInterfaces);
	}

	/*std::vector<ScriptField> ManagedClass::GetEnumFields() const
	{
		bool isEnum = mono_class_is_enum(m_MonoClass);
		TR_ASSERT(isEnum, "Class isn't of type 'enum'");

		std::vector<ScriptField> fields;

		for (const auto& field : m_Fields)
		{
			if (field.IsStatic() && field.GetVisibility() == ScriptFieldVisibility::Public)
				fields.emplace_back(field);
		}

		return fields;
	}*/

	std::vector<ManagedMethod> ManagedClass::GetMethods() const
	{
		std::vector<ManagedMethod> methods;
		
		void* iter = nullptr;
		while(true) 
		{
			MonoMethod* method = mono_class_get_methods(m_MonoClass, &iter);
			if (method)
				methods.push_back({ method });
			else
				break;
		}

		return methods;
	}

	bool ManagedClass::IsEnum() const { return mono_class_is_enum(m_MonoClass); }

	int ManagedClass::GetTypeToken() const
	{
		return mono_class_get_type_token(m_MonoClass);
	}

	ManagedObject ManagedClass::CreateInstance()
	{
		MonoObject* monoObject = mono_object_new(mono_domain_get(), m_MonoClass);
		mono_runtime_object_init(monoObject);

		ManagedObject object = monoObject;
		object.m_Class = this;

		return object;
	}
}
