#include "trpch.h"
#include "ScriptField.h"

#include <mono/metadata/attrdefs.h>

namespace TerranEngine 
{
	static ScriptFieldType ConvertFieldType(MonoType* monoType)
	{
		MonoTypeEnum type = (MonoTypeEnum)mono_type_get_type(monoType);

		switch (type)
		{
		case MONO_TYPE_BOOLEAN:		return ScriptFieldType::Bool;

		// signed integer types
		case MONO_TYPE_I1:			return ScriptFieldType::Int8;
		case MONO_TYPE_I2:			return ScriptFieldType::Int16;
		case MONO_TYPE_I4:			return ScriptFieldType::Int;
		case MONO_TYPE_I8:			return ScriptFieldType::Int64;

		// unsigned integer types
		case MONO_TYPE_U1:			return ScriptFieldType::UInt8;
		case MONO_TYPE_U2:			return ScriptFieldType::UInt16;
		case MONO_TYPE_U4:			return ScriptFieldType::UInt;
		case MONO_TYPE_U8:			return ScriptFieldType::UInt64;

		case MONO_TYPE_R4:			return ScriptFieldType::Float;
		case MONO_TYPE_R8:			return ScriptFieldType::Double;
		case MONO_TYPE_CHAR:		return ScriptFieldType::Char;
		case MONO_TYPE_STRING:		return ScriptFieldType::String;
		case MONO_TYPE_VALUETYPE: 
		{
			MonoClass* typeClass = mono_class_from_mono_type(monoType);
			const char* typeClassTypeName = mono_class_get_name(typeClass);
			const char* typeClassNamespace = mono_class_get_namespace(typeClass);

			// TODO: string comparing is slow, think of a better way to do this
			if (strcmp(typeClassNamespace, "TerranScriptCore") == 0) 
			{
				if (strcmp(typeClassTypeName, "Vector2") == 0)
					return ScriptFieldType::Vector2;
				if (strcmp(typeClassTypeName, "Vector3") == 0)
					return ScriptFieldType::Vector3;
			}
		}
		}

		return ScriptFieldType::None;
	}

	static ScriptFieldVisiblity ConvertFieldVisibility(uint32_t accessMask) 
	{
		switch (accessMask)
		{
		case MONO_FIELD_ATTR_PRIVATE:	return ScriptFieldVisiblity::Private;
		case MONO_FIELD_ATTR_PUBLIC:	return ScriptFieldVisiblity::Public;
		case MONO_FIELD_ATTR_FAMILY:	return ScriptFieldVisiblity::Protected;
		case MONO_FIELD_ATTR_ASSEMBLY:	return ScriptFieldVisiblity::Internal;
		}

		return ScriptFieldVisiblity::Unknown;
	}

	ScriptField::ScriptField(void* monoField, uint32_t monoObjectGCHandle)
		: m_MonoField(monoField), m_MonoObjectGCHandle(monoObjectGCHandle)
	{
		m_Name = mono_field_get_name((MonoClassField*)m_MonoField);
		m_FieldType = ConvertFieldType(mono_field_get_type((MonoClassField*)m_MonoField));

		uint32_t accessMask = mono_field_get_flags((MonoClassField*)m_MonoField) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;
		m_FieldVisibility = ConvertFieldVisibility(accessMask);
	}

	ScriptField::~ScriptField() 
	{
	}

	template<typename T, typename CachedDataType>
	static void ExtractFieldData(void* result, CachedDataType& cachedData, MonoObject* monoObject, MonoClassField* monoField)
	{
		if (monoObject != nullptr && monoField != nullptr)
			cachedData = *(T*)result;

		*(T*)result = cachedData;
	}

	void ScriptField::SetDataRaw(void* value)
	{
		MonoObject* monoObject = mono_gchandle_get_target(m_MonoObjectGCHandle);
		MonoClassField* monoField = (MonoClassField*)m_MonoField;

		if (monoObject == nullptr) 
		{
			TR_ERROR("Couldn't set the value of field {0} because the object that it corresponds to is no longer valid", m_Name);
			return;
		}

		if (monoField == nullptr) 
		{
			TR_ERROR("Couldn't set the value of field {0} because it wasn't found", m_Name);
			return;
		}
		
		if (monoObject != nullptr && monoField != nullptr)
			mono_field_set_value(mono_gchandle_get_target(m_MonoObjectGCHandle), (MonoClassField*)m_MonoField, value);
	}

	void ScriptField::GetDataRaw(void* result)
	{
		MonoObject* monoObject = mono_gchandle_get_target(m_MonoObjectGCHandle);
		MonoClassField* monoField = (MonoClassField*)m_MonoField;

		if (monoObject == nullptr) 
		{
			TR_ERROR("Couldnt find the object");
			return;
		}

		if (monoField == nullptr) 
		{
			TR_ERROR("Mono field is null");
			return;
		}
		
		mono_field_get_value(monoObject, monoField, result);	
	}

	const char* ScriptField::GetDataStringRaw()
	{
		if (m_FieldType != ScriptFieldType::String) 
		{
			TR_ERROR("Can't get the string value of a non-string field");
			return "";
		}

		MonoString* string = nullptr;

		mono_field_get_value(mono_gchandle_get_target(m_MonoObjectGCHandle), (MonoClassField*)m_MonoField, &string);

		return ScriptString(string).GetUTF8Str();
	}

	void ScriptField::SetDataStringRaw(const char* value)
	{
		MonoObject* monoObject = mono_gchandle_get_target(m_MonoObjectGCHandle);
		MonoClassField* monoField = (MonoClassField*)m_MonoField;

		if (monoObject == nullptr) 
		{
			TR_ERROR("Couldnt find the object");
			return;
		}

		if (monoField == nullptr) 
		{
			TR_ERROR("Mono field is null");
			return;
		}

		if (m_FieldType != ScriptFieldType::String)
		{
			TR_ERROR("Can't set the string value of a non-string field");
			return;
		}
		
		ScriptString string((const char*)value);
		mono_field_set_value(monoObject, monoField, string.GetStringInternal());

	}
}
