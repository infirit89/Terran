#pragma once

#include "GCManager.h"
#include "ScriptArray.h"
#include "ScriptType.h"
#include "ScriptUtils.h"

#include "Scene/Entity.h"

#include "Utils/Utils.h"

extern "C"
{
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoProperty MonoProperty;
}

namespace TerranEngine 
{
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

		void CopyData(GCHandle from, GCHandle to);

		
		inline const char* GetName() const					{ return m_Name; }
		inline ScriptFieldVisibility GetVisibility() const	{ return m_FieldVisibility; }
		bool IsStatic() const;

		inline operator bool() const { return m_MonoField != nullptr; }

		inline uint32_t GetID() const { return m_ID; }
		inline const ScriptType& GetType() const { return m_Type; } 
		
		void SetDataStringRaw(const char* value, GCHandle handle);
		std::string GetDataStringRaw(GCHandle handle);

		void SetDataVariantRaw(const Utils::Variant& value, GCHandle handle);
		Utils::Variant GetDataVariantRaw(GCHandle handle);

		void SetDataUUIDRaw(UUID value, GCHandle handle);
		UUID GetDataUUIDRaw(GCHandle handle);

		ScriptArray GetArray(GCHandle handle);
		void SetArray(ScriptArray array, GCHandle handle);

		template<typename T>
		T GetData(GCHandle handle) 
		{
			T value{};
			ScriptUtils::GetFieldDataRaw(&value, m_MonoField, handle);
			return value;
		}

		template<typename T>
		void SetData(T value, GCHandle handle) 
		{
			ScriptUtils::SetFieldDataRaw(&value, m_MonoField, handle);
		}

		template<>
		std::string GetData<std::string>(GCHandle handle) 
		{
			std::string value = GetDataStringRaw(handle);
			return value;
		}

		template<>
		void SetData<const char*>(const char* value, GCHandle handle) 
		{
			SetDataStringRaw(value, handle);
		}

		template<>
		void SetData<Utils::Variant>(Utils::Variant value, GCHandle handle)
		{
			SetDataVariantRaw(value, handle);
		}		

		template<>
		Utils::Variant GetData<Utils::Variant>(GCHandle handle) 
		{
			Utils::Variant value = GetDataVariantRaw(handle);
			return value;
		}

		template<>
		void SetData<UUID>(UUID value, GCHandle handle)
		{
			SetDataUUIDRaw(value, handle);
		}		

		template<>
		UUID GetData<UUID>(GCHandle handle) 
		{
			UUID value = GetDataUUIDRaw(handle);
			return value;
		}
		
		// NOTE: maybe marshal vec2 and vec3?

	private:
		MonoClassField* m_MonoField = nullptr;
		ScriptType m_Type;
		const char* m_Name;
		ScriptFieldVisibility m_FieldVisibility = ScriptFieldVisibility::Unknown;
		uint32_t m_ID = 0;
		
		friend class ScriptCache;
	};
}
