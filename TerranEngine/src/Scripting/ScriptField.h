#pragma once

#include "GCManager.h"

#include "Utils/Utils.h"

extern "C"
{
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoObject MonoObject;
}

namespace TerranEngine 
{
	enum class ScriptFieldType
	{
		None = 0,
		Bool,
		Char,
		Int8,
		Int16,
		Int32,
		Int64,
		UInt8,
		UInt16,
		UInt32,
		UInt64,
		Float,
		Double,
		String,
		Vector2,
		Vector3
	};

	enum class ScriptFieldVisibility 
	{
		Unknown,
		Private,
		Public,
		Protected,
		Internal
	};

	class ScriptField 
	{
	public:
		ScriptField() = default;
		ScriptField(MonoClassField* monoField);
		
		ScriptField(const ScriptField& other) = default;
		~ScriptField() = default;

		inline const std::string& GetName() const			{ return m_Name; }
		inline ScriptFieldType GetType() const				{ return m_FieldType; }
		inline ScriptFieldVisibility GetVisibility() const	{ return m_FieldVisibility; }

		inline operator bool() const { return m_MonoField != nullptr; }

		uint32_t GetID() const { return m_ID; }
		
		void SetDataRaw(void* value, GCHandle handle);
		void GetDataRaw(void* result, GCHandle handle);

		const char* GetDataStringRaw(GCHandle handle);
		void SetDataStringRaw(const char* value, GCHandle handle);

		void SetDataVariantRaw(const Utils::Variant& variant, GCHandle handle);
		
		template<typename T>
		T GetData(GCHandle handle) 
		{
			T value;
			GetDataRaw(&value, handle);
			return value;
		}

		template<typename T>
		void SetData(T value, GCHandle handle) 
		{
			SetDataRaw(&value, handle);
		}

		template<>
		const char* GetData<const char*>(GCHandle handle) 
		{
			const char* value = GetDataStringRaw(handle);
			return value;
		}

		template<>
		void SetData<const char*>(const char* value, GCHandle handle) 
		{
			SetDataStringRaw(value, handle);
		}

		template<>
		void SetData<Utils::Variant>(Utils::Variant variant, GCHandle handle)
		{
			SetDataVariantRaw(variant, handle);
		}		

		// NOTE: maybe marshal vec2 and vec3?

	private:
		MonoClassField* m_MonoField = nullptr;
		std::string m_Name;
		ScriptFieldType m_FieldType = ScriptFieldType::None;
		ScriptFieldVisibility m_FieldVisibility = ScriptFieldVisibility::Unknown;
		uint32_t m_ID;

		friend class ScriptCache;
	};
}
