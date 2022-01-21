#pragma once

#include "ScriptString.h"

#include <mono/metadata/class.h>
#include <mono/metadata/object.h>

namespace TerranEngine 
{
	enum class ScriptFieldType 
	{
		Unknown = -1,
		None = 0,
		Bool,
		Char,
		Int,
		/*TODO: 
			int8, int16, int64
			uint8, uint16, uint32, uint64
		*/ 
		Float,
		Double,
		String
	};

	class ScriptField 
	{
	public:
		ScriptField(MonoClassField* monoField, MonoObject* monoObject);

		inline const char* GetName() const 
		{
			return m_Name;
		}

		inline ScriptFieldType GetType() const 
		{
			return m_FieldType;
		}

		template <typename T>
		void Set(T value) 
		{
			mono_field_set_value(m_MonoObject, m_MonoField, &value);
		}

		template <>
		void Set<const char*>(const char* value) 
		{
			ScriptString string(value);
			mono_field_set_value(m_MonoObject, m_MonoField, string.GetStringInternal());
		}

		template <typename T>
		T Get() 
		{
			T value;
			mono_field_get_value(m_MonoObject, m_MonoField, &value);
			return value;
		}

		template <>
		const char* Get<const char*>() 
		{
			MonoString* string = nullptr;

			mono_field_get_value(m_MonoObject, m_MonoField, &string);
			return ScriptString(string).GetUTF8Str();
		}

	private:
		MonoClassField* m_MonoField = nullptr;
		MonoObject* m_MonoObject = nullptr;
		const char* m_Name = nullptr;
		ScriptFieldType m_FieldType = ScriptFieldType::None;
	};
}