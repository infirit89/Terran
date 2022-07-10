#include "trpch.h"
#include "ScriptMethod.h"

#include "ScriptObject.h"

#include <mono/jit/jit.h>
#include <mono/metadata/debug-helpers.h>

#include "GCManager.h"
#include "ScriptUtils.h"

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

	ScriptObject ScriptMethod::Invoke(ScriptObject& scriptObject, void** args)
	{
		MonoException* exc;
		size_t paramsSize = 0;

		// NOTE: this causes an access violation when a null reference occurs on the c# side
		// going to leave the "break when this exception occurs" on for now 
		// if there's a problem in the future this may be the cause
		MonoObject* rtVal = mono_runtime_invoke(m_MonoMethod, scriptObject.GetMonoObject(),
												args, (MonoObject**)&exc);

		if (exc != nullptr) 
		{
			ScriptUtils::PrintUnhandledException(exc);
			return nullptr;
		}

		return rtVal;
	}

	ScriptObject ScriptMethod::InvokeStatic(void** args)
	{
		MonoException* exc;
		size_t paramsSize = 0;

		// NOTE: this causes an access violation when a null reference occurs on the c# side
		// going to leave the "break when this exception occurs" on for now 
		// if there's a problem in the future this may be the cause
		MonoObject* rtVal = mono_runtime_invoke(m_MonoMethod, nullptr, args, (MonoObject**)&exc);

		if (exc != nullptr)
		{
			ScriptUtils::PrintUnhandledException(exc);
			return nullptr;
		}

		return rtVal;
	}


}
