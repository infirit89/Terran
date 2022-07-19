#pragma once

#include <string>

extern "C"
{
	typedef struct _MonoMethod MonoMethod;
}

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

		ScriptObject Invoke(ScriptObject& scriptObject, void** args);
		ScriptObject InvokeStatic(void** args);

		inline MonoMethod* GetMonoMethod() const { return m_MonoMethod; }

		operator bool() const { return m_MonoMethod != nullptr; }

		inline const std::string& GetName() const { return m_Name; }
	private:
		MonoMethod* m_MonoMethod = nullptr;
		std::string m_Name;
	};
} 