#include "trpch.h"
#include "ScriptMarshal.h"
#include "ScriptCache.h"

#include <mono/metadata/appdomain.h>

namespace TerranEngine 
{
	MonoArray* ScriptMarshal::UUIDToMonoArray(const UUID& id)
	{
		const uint8_t* idData = id.GetRaw();
		MonoClass* byteClass = TR_CORE_CACHED_CLASS(Byte)->GetMonoClass();
		MonoArray* uuidArray = mono_array_new(mono_domain_get(), byteClass, 16);
		
		uint8_t* uuidArrayAddr = mono_array_addr(uuidArray, uint8_t, 0);
		
		memcpy(uuidArrayAddr, idData, 16 * sizeof(uint8_t));

		return uuidArray;
	}

	UUID ScriptMarshal::MonoArrayToUUID(MonoArray* uuidArray)
	{
		if (mono_array_length(uuidArray) != 16) 
		{
			TR_ERROR("Mono UUID array is invalid");
			TR_TRACE(mono_array_length(uuidArray));
			return UUID({ 0 });
		}

		std::array<uint8_t, 16> idData = { 0 };

		const uint8_t* uuidArrayAddr = mono_array_addr(uuidArray, uint8_t, 0);

		memcpy(idData._Elems, uuidArrayAddr, 16 * sizeof(uint8_t));

		return UUID(idData);
	}

	MonoString* ScriptMarshal::UTF8ToMonoString(const std::string& str)
	{
		MonoDomain* mainDomain = mono_domain_get();
		return mono_string_new(mainDomain, str.c_str());
	}

	std::string ScriptMarshal::MonoStringToUTF8(MonoString* monoStr)
	{
		MonoError error;
		const char* str = mono_string_to_utf8_checked(monoStr, &error);

		if (error.error_code != MONO_ERROR_NONE) 
		{
			TR_ERROR("Mono Error ID: {0}; Message {1}", mono_error_get_error_code(&error), mono_error_get_message(&error));
			return "";
		}

		std::string result(str);
		mono_free((char*)str);

		return result;
	}
}
