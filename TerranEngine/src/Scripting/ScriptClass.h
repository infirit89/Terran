#pragma once

#include "ScriptMethod.h"

typedef struct _MonoClass MonoClass;
namespace TerranEngine 
{
	class ScriptClass 
	{
	public:
		ScriptClass() = default;
		ScriptClass(MonoClass* monoClass);

		ScriptClass(const ScriptClass& other) = default;
		~ScriptClass() = default;

		ScriptMethod GetMethod(const char* methodSignature);

		inline MonoClass* GetMonoClass() const { return m_MonoClass; }
		inline const char* GetName() const { return m_ClassName; }
		inline const char* GetNamespace() const { return m_Namespace; }

		ScriptClass GetParent();
		bool operator==(const ScriptClass& other) const { return m_MonoClass == other.m_MonoClass; }

		bool IsInstanceOf(ScriptClass* parent, bool checkInterfaces = true);

	private:
		MonoClass* m_MonoClass = nullptr;
		const char* m_ClassName = nullptr;
		const char* m_Namespace = nullptr;
	};
}
