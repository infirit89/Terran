#pragma once

#include "Core/Base.h"

#include "ScriptMethod.h"
#include "ScriptField.h"

#include <unordered_map>

#include <mono/metadata/object.h>

namespace TerranEngine 
{
	class ScriptObject 
	{
	public:
		ScriptObject() = default;

		ScriptObject(MonoObject* monoObject, std::unordered_map<uint32_t, Shared<ScriptMethod>>& methods);

		void Execute(const char* methodName, ScriptMethodParameterList parameterList = {});

		inline MonoObject* GetNativeObject() const { return m_MonoObject; }

		Shared<ScriptField> GetField(const char* fieldName);
	private:
		MonoObject* m_MonoObject;

		std::unordered_map<uint32_t, Shared<ScriptField>> m_Fields;
		std::unordered_map<uint32_t, Shared<ScriptMethod>> m_Methods;
	};
}
