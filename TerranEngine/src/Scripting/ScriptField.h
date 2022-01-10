#pragma once

#include <mono/metadata/class.h>
#include <mono/metadata/object.h>

namespace TerranEngine 
{
	class ScriptField 
	{
	public:
		ScriptField(MonoClassField* monoField, MonoObject* monoObject);

		template <typename T>
		void Set(T value) 
		{
			mono_field_set_value(m_MonoObject, m_MonoField, &value);
		}

		template <typename T>
		T Get() 
		{
			T value;
			mono_field_get_value(m_MonoObject, m_MonoField, &value);
			return value;
		}

	private:
		MonoClassField* m_MonoField = nullptr;
		MonoObject* m_MonoObject = nullptr;
	};
}