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
		String
	};

	class ScriptField 
	{
		union FieldCacheData 
		{
			double dValue;
			int64_t iValue;
			bool bValue;

			// TODO: add string

			operator bool()		{ return bValue; }

			operator int8_t()	{ return static_cast<int8_t>(iValue); }
			operator int16_t()	{ return static_cast<int16_t>(iValue); }
			operator int32_t()	{ return static_cast<int32_t>(iValue); }
			operator int64_t()	{ return static_cast<int64_t>(iValue); }

			operator uint8_t()	{ return static_cast<uint8_t>(iValue); }
			operator uint16_t() { return static_cast<uint16_t>(iValue); }
			operator uint32_t() { return static_cast<uint32_t>(iValue); }
			operator uint64_t() { return static_cast<uint64_t>(iValue); }

			operator float()	{ return static_cast<float>(dValue); }
			operator double()	{ return static_cast<double>(dValue); }
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