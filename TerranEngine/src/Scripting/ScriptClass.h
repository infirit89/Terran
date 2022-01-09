#pragma once

#include "ScriptMethod.h"
#include "Core/Base.h"

#include <unordered_map>

#include <mono/metadata/class.h>

namespace TerranEngine 
{
	class ScriptClass 
	{
	public:
		ScriptClass(MonoClass* monoClass);

		void ExecuteStatic(const char* methodName);

	private:
		MonoClass* m_MonoClass;
		std::unordered_map<uint32_t, Shared<ScriptMethod>> m_Methods;
	};
}