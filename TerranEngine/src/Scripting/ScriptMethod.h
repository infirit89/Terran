#pragma once

#include "Core/Base.h"

typedef struct _MonoMethod MonoMethod;

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