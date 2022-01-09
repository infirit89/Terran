#include "trpch.h"
#include "ScriptMethod.h"

#include <mono/jit/jit.h>

namespace TerranEngine 
{
	ScriptMethod::ScriptMethod(MonoMethod* monoMethod)
		: m_MonoMethod(monoMethod)
	{
	}

	void ScriptMethod::Execute(ScriptObject* scriptObject, ScriptMethodParameterList parameterList)
	{
		MonoObject* error;
		size_t paramsSize = 0;

		void** args = parameterList.GetRawParams();
		mono_runtime_invoke(m_MonoMethod, scriptObject == nullptr ? nullptr : scriptObject->GetNativeObject(), args, &error);

		delete[] args;

		if (error != nullptr)
			// TODO: should report the error name and code
			TR_ERROR("error");
	}
}

