#include "trpch.h"
#include "ScriptMarshal.h"

#include <mono/metadata/appdomain.h>

namespace TerranEngine 
{
	MonoArray* ScriptMarshal::UUIDToMonoArray(const UUID& id)
	{
		const uint8_t* idData = id.GetRaw();
		MonoArray* uuidArray = mono_array_new(mono_domain_get(), mono_get_byte_class(), 16);

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
}
