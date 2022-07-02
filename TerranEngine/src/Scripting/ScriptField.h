#pragma once

#include "GCManager.h"
#include "ScriptType.h"
#include "Scene/Entity.h"

#include "Utils/Utils.h"

extern "C"
{
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoType MonoType;
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

		inline const std::string& GetName() const			{ return m_Name; }
		inline ScriptFieldVisibility GetVisibility() const	{ return m_FieldVisibility; }

		inline operator bool() const { return m_MonoField != nullptr; }

		inline uint32_t GetID() const { return m_ID; }
		inline const ScriptType& GetType() const { return m_Type; } 
		
		void SetDataRaw(void* value, GCHandle handle);
		void GetDataRaw(void* result, GCHandle handle);

		void SetDataStringRaw(const char* value, GCHandle handle);
		std::string GetDataStringRaw(GCHandle handle);

		void SetDataVariantRaw(const Utils::Variant& value, GCHandle handle);
		Utils::Variant GetDataVariantRaw(GCHandle handle);

		void SetDataEntityRaw(Entity value, GCHandle handle);
		Entity GetDataEntityRaw(GCHandle handle);
		
		template<typename T>
		T GetData(GCHandle handle) 
		{
			T value{};
			GetDataRaw(&value, handle);
			return value;
		}

		template<typename T>
		void SetData(T value, GCHandle handle) 
		{
			SetDataRaw(&value, handle);
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
		void SetData<Entity>(Entity value, GCHandle handle)
		{
			SetDataEntityRaw(value, handle);
		}		

		template<>
		Entity GetData<Entity>(GCHandle handle) 
		{
			Entity value = GetDataEntityRaw(handle);
			return value;
		}
		
		
		// NOTE: maybe marshal vec2 and vec3?

	private:
		MonoClassField* m_MonoField = nullptr;
		ScriptType m_Type;
		std::string m_Name;
		ScriptFieldVisibility m_FieldVisibility = ScriptFieldVisibility::Unknown;
		uint32_t m_ID = 0;

		friend class ScriptCache;
	};
}
