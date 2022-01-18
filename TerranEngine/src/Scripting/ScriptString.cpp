#include "trpch.h"
#include "ScriptString.h"

#include <mono/metadata/appdomain.h>

namespace TerranEngine 
{
	ScriptString::ScriptString(MonoString* monoString)
		: m_MonoString(monoString)
	{
		MonoError* error = nullptr;

		m_StrBuffer = mono_string_to_utf8_checked(m_MonoString, error);

		if (!error)
			TR_ERROR("Mono Error ID: {0}; Message {1}", mono_error_get_error_code(error), mono_error_get_message(error));
	}

	ScriptString::ScriptString(const char* string)
		: m_StrBuffer(string), m_MonoString(mono_string_new(mono_domain_get(), string)) {}

	ScriptString::~ScriptString()
	{
		mono_free((void*)m_StrBuffer);
	}
}
