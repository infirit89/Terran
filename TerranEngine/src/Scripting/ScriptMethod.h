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

		void Invoke(ScriptObject& scriptObject, void** args);
		
		void InvokeStatic(void** args);

		inline MonoMethod* GetNativeMethodPtr() const { return m_MonoMethod; }
	private:
		MonoMethod* m_MonoMethod = nullptr;
	};
} 