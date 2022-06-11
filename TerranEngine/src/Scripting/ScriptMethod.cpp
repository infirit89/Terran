#include "trpch.h"
#include "ScriptMethod.h"

#include "ScriptObject.h"

#include <mono/jit/jit.h>
#include <mono/metadata/debug-helpers.h>

#include "GCManager.h"

namespace TerranEngine 
{
	ScriptMethod::ScriptMethod(MonoMethod* monoMethod)
		: m_MonoMethod(monoMethod)
	{
		if(monoMethod)
		{
			m_Name = mono_method_get_name(m_MonoMethod);
		}
	}

	void ScriptMethod::Invoke(ScriptObject& scriptObject, void** args)
	{
		MonoObject* error;
		size_t paramsSize = 0;

		// NOTE: this causes an access violation when a null reference occurs on the c# side
		// going to leave the "break when this exception occurs" on for now 
		// if there's a problem in the future this may be the cause
		mono_runtime_invoke(m_MonoMethod, scriptObject.GetMonoObject(), args, &error);

		if (error != nullptr) 
		{
			MonoClass* klass = mono_object_get_class(error);
			TR_ERROR("Exception {0} caused by {1} method", mono_class_get_name(klass), mono_method_get_name(m_MonoMethod));
		}
	}

	void ScriptMethod::InvokeStatic(void** args)
	{
		MonoObject* error;
		size_t paramsSize = 0;

		// NOTE: this causes an access violation when a null reference occurs on the c# side
		// going to leave the "break when this exception occurs" on for now 
		// if there's a problem in the future this may be the cause
		mono_runtime_invoke(m_MonoMethod, nullptr, args, &error);

		if (error != nullptr)
		{
			MonoClass* klass = mono_object_get_class(error);
			TR_ERROR("Exception {0} caused by {1} method", mono_class_get_name(klass), mono_method_get_name(m_MonoMethod));
		}

	}


}
