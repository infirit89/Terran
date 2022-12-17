#pragma once

#include "Core/PlatformCheck.h"

#include "ScriptMethod.h"

#include "Utils/Debug/OptickProfiler.h"

#include <mono/metadata/object.h>
#include <mono/metadata/exception.h>

#include <format>

namespace TerranEngine 
{
	#ifdef TR_WINDOWS_64
	#define DEFAULT_CALL_C __stdcall
	#else
	#define DEFAULT_CALL_C 
	#endif // TR_WINDOWS_64

	template<typename... TParameters>
	class ScriptMethodThunks 
	{
		typedef void(DEFAULT_CALL_C* M)(MonoObject* obj, TParameters... params, MonoException** exc);

	public:
		ScriptMethodThunks() = default;

		void SetFromMethod(ScriptMethod* method) 
		{
			TR_ASSERT(method, "Invalid method");

			m_MethodThunk = (M)mono_method_get_unmanaged_thunk(method->GetMonoMethod());
		}

		void Invoke(MonoObject* obj, TParameters... params, MonoException** exc) 
		{
			TR_PROFILE_FUNCTION();

			if(m_MethodThunk != nullptr)
				m_MethodThunk(obj, params..., exc);
		}
		 
		operator bool() { return m_MethodThunk != nullptr; }

	private:
		M m_MethodThunk = nullptr;
	};

	template<typename TReturn, typename... TParameters>
	class ScriptMethodThunksR
	{
		typedef TReturn(DEFAULT_CALL_C* M)(MonoObject* obj, TParameters... params, MonoException** exc);

	public:
		ScriptMethodThunksR() = default;

		void SetFromMethod(ScriptMethod* method)
		{
			if (method == nullptr)
				return;

			m_MethodThunk = (M)mono_method_get_unmanaged_thunk(method->GetMonoMethod());
		}

		TReturn Invoke(MonoObject* obj, TParameters... params, MonoException** exc)
		{
			return m_MethodThunk(obj, params..., exc);
		}

		operator bool() { return m_MethodThunk != nullptr; }

	private:
		M m_MethodThunk = nullptr;
	};
}
