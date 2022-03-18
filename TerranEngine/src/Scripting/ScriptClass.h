#pragma once

#include "ScriptMethod.h"
#include "ScriptObject.h"

#include "Core/Base.h"

#include <mono/metadata/class.h>

#include <unordered_map>

namespace TerranEngine 
{
	class ScriptClass 
	{
	public:
		ScriptClass() = default;
		ScriptClass(MonoClass* monoClass);

		ScriptClass(const ScriptClass& other) = default;
		~ScriptClass() = default;

		ScriptObject CreateInstance();

		ScriptMethod GetMethod(const char* methodSignature);

		inline MonoClass* GetNativeClassPtr() const { return m_MonoClass; }
		inline const char* GetName() const { return m_ClassName; }
		inline const char* GetNamespace() const { return m_Namespace; }

		ScriptClass GetParent();

	private:
		MonoClass* m_MonoClass = nullptr;
		const char* m_ClassName = nullptr;
		const char* m_Namespace = nullptr;
	};
}