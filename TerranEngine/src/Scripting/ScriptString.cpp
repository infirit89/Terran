#include "trpch.h"
#include "ScriptString.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>

namespace TerranEngine 
{
	ScriptString::ScriptString(MonoString* monoString)
		: m_MonoString(monoString)
	{
		MonoError error;
		error.error_code = MONO_ERROR_NONE;

		m_StrBuffer = mono_string_to_utf8_checked(m_MonoString, &error);

		if(error.error_code != MONO_ERROR_NONE)
			TR_ERROR("Mono Error ID: {0}; Message {1}", mono_error_get_error_code(&error), mono_error_get_message(&error));

		m_MonoAllocatedBuffer = true;
	}

	ScriptString::ScriptString(const char* string)
		: m_MonoString(mono_string_new(mono_domain_get(), string)) 
	{
		//MonoError error;
		//error.error_code = MONO_ERROR_NONE;

		//m_StrBuffer = mono_string_to_utf8_checked(m_MonoString, &error);

		//if (error.error_code != MONO_ERROR_NONE)
		//	TR_ERROR("Mono Error ID: {0}; Message {1}", mono_error_get_error_code(&error), mono_error_get_message(&error));
	}

	ScriptString::~ScriptString()
	{
		if(m_MonoAllocatedBuffer)
			mono_free((void*)m_StrBuffer);
	}
}
