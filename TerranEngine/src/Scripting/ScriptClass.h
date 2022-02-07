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
		ScriptClass(MonoClass* monoClass);

		Shared<ScriptObject> CreateInstance();

		Shared<ScriptMethod> GetMethod(const char* methodSignature);

	private:
		MonoClass* m_MonoClass;
	};
}