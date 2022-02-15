#pragma once

#include "Core/Base.h"

#include <mono/metadata/debug-helpers.h>

namespace TerranEngine 
{
	class ScriptObject;

	class ScriptMethod 
	{
	public:
		ScriptMethod() = default;
		ScriptMethod(MonoMethod* monoMethod);
		ScriptMethod(const ScriptMethod& other) = default;

		~ScriptMethod() = default;

		void Invoke(Shared<ScriptObject> scriptObject, void** args);
		
	private:
		MonoMethod* m_MonoMethod = nullptr;
	};
} 