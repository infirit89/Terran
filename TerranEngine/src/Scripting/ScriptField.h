#pragma once

#include "ScriptString.h"

namespace TerranEngine 
{
	enum class ScriptFieldType
	{
		None = 0,
		Bool,
		Char,
		Int,
		Int8,
		Int16,
		Int64,
		UInt8,
		UInt16,
		UInt,
		UInt64,
		Float,
		Double,
		String,
		Vector2,
		Vector3
	};

	enum class ScriptFieldVisiblity 
	{
		Unknown,
		Private,
		Public,
		Protected,
		Internal
	};

	// NOTE: templates?
	class ScriptField 
	{
	public:
		ScriptField() = default;
		ScriptField(void* monoField, uint32_t monoObjectGCHandle);
		
		ScriptField(const ScriptField& other) = default;
		~ScriptField();

		inline const char* GetName() const					{ return m_Name; }
		inline ScriptFieldType GetType() const				{ return m_FieldType; }
		inline ScriptFieldVisiblity GetVisibility() const					{ return m_FieldVisibility; }

		void SetDataRaw(void* value);
		void GetDataRaw(void* result);

		const char* GetDataStringRaw();
		void SetDataStringRaw(const char* value);

		template<typename T>
		T GetData() 
		{
			T value;
			GetDataRaw(&value);
			return value;
		}

		template<typename T>
		void SetData(T value) 
		{
			SetDataRaw(&value);
		}

		template<>
		const char* GetData<const char*>() 
		{
			const char* value = GetDataStringRaw();
			return value;
		}

		template<>
		void SetData<const char*>(const char* value) 
		{
			SetDataStringRaw(value);
		}

		// NOTE: maybe marshal vec2 and vec3

	private:
		void* m_MonoField = nullptr;
		uint32_t m_MonoObjectGCHandle = 0;
		const char* m_Name = nullptr;
		ScriptFieldType m_FieldType = ScriptFieldType::None;
		ScriptFieldVisiblity m_FieldVisibility = ScriptFieldVisiblity::Unknown;
	};
}