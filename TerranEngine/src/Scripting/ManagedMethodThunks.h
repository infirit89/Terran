#pragma once

#include "Core/PlatformCheck.h"

#include "ManagedMethod.h"

#include "Utils/Debug/OptickProfiler.h"

#include <mono/metadata/object.h>
#include <mono/metadata/exception.h>

//#include <format>

namespace TerranEngine 
{
	#ifdef TR_WINDOWS_64
	#define DEFAULT_CALL_C __stdcall
	#else
	#define DEFAULT_CALL_C 
	#endif // TR_WINDOWS_64

	template<typename... TParameters>
	class ManagedMethodThunks
	{
		typedef void(DEFAULT_CALL_C* M)(MonoObject* obj, TParameters... params, MonoException** exc);

	public:
		ManagedMethodThunks() = default;

		void SetFromMethod(ManagedMethod* method) 
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
	class ManagedMethodThunksR
	{
		typedef TReturn(DEFAULT_CALL_C* M)(MonoObject* obj, TParameters... params, MonoException** exc);

	public:
		ManagedMethodThunksR() = default;

		void SetFromMethod(ManagedMethod* method)
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
