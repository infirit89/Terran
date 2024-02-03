#pragma once

#include "ScriptType.h"

#include "Scene/Entity.h"

#include <vector>

namespace Coral 
{
	class Type;
	class FieldInfo;
}

namespace TerranEngine 
{
	struct ScriptField
	{
		ScriptType Type;
		std::string Name;
	};

	class ScriptInstance 
	{
	public:
		ScriptInstance(Coral::Type& type, Entity entity);

		template<typename TValue>
		void SetFieldValue(int32_t fieldHandle, TValue value) const
		{
			SetFieldValueInternal(fieldHandle, &value);
		}

		template<typename TReturn>
		TReturn GetFieldValue(int32_t fieldHandle) const 
		{
			TReturn value;
			GetFieldValueInternal(fieldHandle, &value);
			return value;
		}

		const ScriptField& GetScriptField(int32_t fieldHandle) const { return m_Fields.at(fieldHandle); }

	private:
		void SetFieldValueInternal(int32_t fieldHandle, void* value) const;
		void GetFieldValueInternal(int32_t fieldHandle, void* value) const;

	private:
		const void* m_Context;
		int32_t m_OnInitMethodHandle;
		int32_t m_OnUpdateMethodHandle;
		std::unordered_map<int32_t, ScriptField> m_Fields;

		friend class ScriptEngine;
	};
}
