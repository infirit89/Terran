#pragma once

#include "ScriptMethod.h"
#include "ScriptObject.h"
#include "Core/Base.h"

#include <unordered_map>

#include <mono/metadata/class.h>

namespace TerranEngine 
{
	class ScriptClass 
	{
	public:
		ScriptClass(MonoClass* monoClass);

		uint32_t CreateInstance();

		Shared<ScriptMethod> GetMethod(const char* methodSignature);

	private:
		MonoClass* m_MonoClass;
	};
}