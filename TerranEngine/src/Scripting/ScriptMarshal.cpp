#include "trpch.h"
#include "ScriptMarshal.h"
#include "ScriptCache.h"

#include <mono/metadata/appdomain.h>

namespace TerranEngine 
{
	ScriptArray ScriptMarshal::UUIDToMonoArray(const UUID& id)
	{
		const uint8_t* idData = id.GetRaw();
		ScriptArray uuidArray = ScriptArray::Create<uint8_t>(16);

		constexpr uint32_t uuidArrayLength = 16 * sizeof(uint8_t); 
		
		memcpy((uint8_t*)uuidArray.GetElementAddress(0, sizeof(uint8_t)), idData, uuidArrayLength);

		return uuidArray;
	}

	UUID ScriptMarshal::MonoArrayToUUID(ScriptArray uuidArray)
	{
		if (uuidArray.Length() != 16)
		{
			TR_ERROR("Mono UUID array is invalid");
			return UUID({ 0 });
		}

		std::array<uint8_t, 16> idData = { 0 };

		const uint8_t* uuidArrayAddr = (uint8_t*)uuidArray.GetElementAddress(0, sizeof(uint8_t));

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
