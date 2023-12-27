#include "trpch.h"
#include "ManagedMethod.h"

#include "ManagedObject.h"

#include <mono/jit/jit.h>
#include <mono/metadata/debug-helpers.h>

#include "GCManager.h"
#include "ScriptUtils.h"

namespace TerranEngine 
{
	ManagedMethod::ManagedMethod(MonoMethod* monoMethod)
		: m_MonoMethod(monoMethod)
	{
		if(monoMethod)
		{
			m_Name = mono_method_get_name(m_MonoMethod);
		}
	}

	ManagedObject ManagedMethod::Invoke(ManagedObject& scriptObject, void** args)
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
			TR_ERROR(ScriptUtils::GetExceptionMessage(exc));
			return nullptr;
		}

		return rtVal;
	}

	ManagedObject ManagedMethod::InvokeStatic(void** args)
	{
		MonoException* exc;
		size_t paramsSize = 0;

		// NOTE: this causes an access violation when a null reference occurs on the c# side
		// going to leave the "break when this exception occurs" on for now 
		// if there's a problem in the future this may be the cause
		MonoObject* rtVal = mono_runtime_invoke(m_MonoMethod, nullptr, args, (MonoObject**)&exc);

		if (exc != nullptr)
		{
			TR_ERROR(ScriptUtils::GetExceptionMessage(exc));
			return nullptr;
		}

		return rtVal;
	}


}
