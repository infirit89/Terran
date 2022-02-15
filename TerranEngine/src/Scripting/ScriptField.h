#pragma once

#include "ScriptString.h"

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

	enum class ScirptFieldVisibility 
	{
		None = 0,
		Private,
		Protected,
		Internal,
		Public
	};

	class ScriptField 
	{
	public:
		ScriptField() = default;
		ScriptField(void* monoField, uint32_t monoObjectGCHandle);

		inline const char* GetName() const					{ return m_Name; }
		inline ScriptFieldType GetType() const				{ return m_FieldType; }
		inline ScirptFieldVisibility GetVisibility() const	{ return m_FieldVisibility; }

		void SetValue(void* value);
		void GetValue(void* result);

		const char* GetValue();
		void SetValue(const char* value);

	private:
		void* m_MonoField = nullptr;
		uint32_t m_MonoObjectGCHandle = 0;
		const char* m_Name = nullptr;
		ScriptFieldType m_FieldType = ScriptFieldType::None;
		ScirptFieldVisibility m_FieldVisibility = ScirptFieldVisibility::None;
	};
}