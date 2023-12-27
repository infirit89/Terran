#pragma once

#include <string>

extern "C"
{
	typedef struct _MonoMethod MonoMethod;
}

namespace TerranEngine 
{
	class ManagedObject;

	class ManagedMethod 
	{
	public:
		ManagedMethod() = default;
		ManagedMethod(MonoMethod* monoMethod);

		ManagedMethod(const ManagedMethod& other) = default;
		~ManagedMethod() = default;

		ManagedObject Invoke(ManagedObject& scriptObject, void** args);
		ManagedObject InvokeStatic(void** args);

		inline MonoMethod* GetMonoMethod() const { return m_MonoMethod; }

		operator bool() const { return m_MonoMethod != nullptr; }

		inline const std::string& GetName() const { return m_Name; }

	private:
		MonoMethod* m_MonoMethod = nullptr;
		std::string m_Name;
	};
} 