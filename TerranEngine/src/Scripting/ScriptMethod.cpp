#include "trpch.h"
#include "ScriptMethod.h"

#include <mono/jit/jit.h>

namespace TerranEngine 
{
	ScriptMethod::ScriptMethod(MonoMethod* monoMethod)
		: m_MonoMethod(monoMethod)
	{
	}

	void ScriptMethod::Execute()
	{
		MonoObject* error;
		mono_runtime_invoke(m_MonoMethod, nullptr, nullptr, &error);

		if (error != nullptr)
			// TODO: should report the error name and code
			TR_ERROR("error");
	}
}

