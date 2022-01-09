#pragma once

#include "Core/Base.h"

#include <unordered_map>

#include <mono/metadata/object.h>

namespace TerranEngine 
{
	class ScriptMethod;
	class ScriptObject 
	{
	public:
		ScriptObject(MonoObject* monoObject, std::unordered_map<uint32_t, Shared<ScriptMethod>>& methods);

		void Execute(const char* methodName);

		inline MonoObject* GetNativeObject() const { return m_MonoObject; }
	private:
		MonoObject* m_MonoObject;
		std::unordered_map<uint32_t, Shared<ScriptMethod>> m_Methods;
	};
}