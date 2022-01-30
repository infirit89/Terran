#include "trpch.h"
#include "ScriptMethod.h"
#include "ScriptObject.h"

#include <mono/jit/jit.h>

namespace TerranEngine 
{
	ScriptMethod::ScriptMethod(MonoMethod* monoMethod)
		: m_MonoMethod(monoMethod)
	{
	}

	ScriptMethod::~ScriptMethod()
	{
		mono_free_method(m_MonoMethod);
	}

	void ScriptMethod::Execute(ScriptObject* scriptObject, ScriptMethodParameterList parameterList)
	{
		MonoObject* error;
		size_t paramsSize = 0;

		void** args = parameterList.GetRawParams();
		mono_runtime_invoke(m_MonoMethod, scriptObject == nullptr ? nullptr : scriptObject->GetNativeObject(), args, &error);
		
		// memory leak?

		if (error != nullptr) 
		{
			MonoClass* klass = mono_object_get_class(error);
			TR_ERROR("Exception {0} caused by {1} method", mono_class_get_name(klass), mono_method_get_name(m_MonoMethod));
		}

		delete[] args;
	}
}

