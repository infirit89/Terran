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
	class ScriptField;

	namespace detail 
	{
		template<typename T>
        T ScriptFieldGetData(GCHandle handle, ScriptField* field);
		template<typename T>
        void ScriptFieldSetData(T value, GCHandle handle, ScriptField* field);
		template<>
        std::string ScriptFieldGetData<std::string>(GCHandle handle, ScriptField* field);
		template<>
        void ScriptFieldSetData<const char*>(const char* value, GCHandle handle, ScriptField* field);
		template<>
        void ScriptFieldSetData<Utils::Variant>(Utils::Variant value, GCHandle handle, ScriptField* field);
		template<>
        Utils::Variant ScriptFieldGetData<Utils::Variant>(GCHandle handle, ScriptField* field);
		template<>
        void ScriptFieldSetData<UUID>(UUID value, GCHandle handle, ScriptField* field);
		template<>
        UUID ScriptFieldGetData<UUID>(GCHandle handle, ScriptField* field);
	}

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

		inline MonoClassField* GetMonoField() const 		{ return m_MonoField; }
		inline const char* GetName() const					{ return m_Name; }
		inline ScriptFieldVisibility GetVisibility() const	{ return m_FieldVisibility; }
		bool IsStatic() const;

		inline operator bool() const { return m_MonoField != nullptr; }

		inline uint32_t GetID() const { return m_ID; }
		inline const ScriptType& GetType() const { return m_Type; } 

		ScriptArray GetArray(GCHandle handle);
		void SetArray(ScriptArray array, GCHandle handle);

		template<typename T>
		T GetData(GCHandle handle)
		{
			return detail::ScriptFieldGetData<T>(handle, this);
		}

		template<typename T>
		void SetData(T value, GCHandle handle) 
		{
			detail::ScriptFieldSetData(value, handle, this);
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

	namespace detail 
	{
		template<typename T>
        T ScriptFieldGetData(GCHandle handle, ScriptField* field) 
        {
            T value{};
            ScriptUtils::GetFieldDataRaw(&value, field->GetMonoField(), handle);
            return value;
        }

        template<typename T>
        void ScriptFieldSetData(T value, GCHandle handle, ScriptField* field) 
        {
            ScriptUtils::SetFieldDataRaw(&value, field->GetMonoField(), handle);
        }
	}
}
