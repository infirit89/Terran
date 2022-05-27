#pragma once

#include "Core/UUID.h"
#include <mono/metadata/object.h>

#include <string>

namespace TerranEngine 
{
	class ScriptMarshal
	{
	public:
		static MonoArray* UUIDToMonoArray(const UUID& id);
		static UUID MonoArrayToUUID(MonoArray* uuidArray);

		static MonoString* UTF8ToMonoString(const std::string& str);
		static std::string MonoStringToUTF8(MonoString* monoStr);
	};
}