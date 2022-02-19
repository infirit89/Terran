#pragma once

#include "Core/Base.h"

#include "ScriptMethod.h"
#include "ScriptObject.h"

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

	private:
		MonoClass* m_MonoClass;
	};
}