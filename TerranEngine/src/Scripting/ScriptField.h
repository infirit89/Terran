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
		Int8,
		Int16,
		Int64,
		UInt8,
		Uint16,
		UInt,
		UInt64,
		Float,
		Double,
		String
	};

	class ScriptField 
	{
		union FieldCacheData 
		{

			double dValue;
			int64_t iValue;
			bool bValue;
		};

	public:
		ScriptField() = default;
		ScriptField(void* monoField, uint32_t monoObjectGCHandle);
		
		ScriptField(const ScriptField& other) = default;
		~ScriptField() = default;

		inline const char* GetName() const					{ return m_Name; }
		inline ScriptFieldType GetType() const				{ return m_FieldType; }

		void SetValue(void* value);
		void GetValue(void* result);

		const char* GetValue();
		void SetValue(const char* value);

		FieldCacheData GetCachedData() { return m_CachedData; }

	private:
		void* m_MonoField = nullptr;
		uint32_t m_MonoObjectGCHandle = 0;
		const char* m_Name = nullptr;
		ScriptFieldType m_FieldType = ScriptFieldType::None;

		FieldCacheData m_CachedData;
	};
}