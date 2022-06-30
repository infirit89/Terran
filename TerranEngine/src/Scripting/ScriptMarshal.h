#pragma once

#include "ScriptArray.h"

#include "Core/UUID.h"

#include <string>

extern "C"
{
	typedef struct _MonoString MonoString;
}

namespace TerranEngine 
{
	class ScriptMarshal
	{
	public:
		static ScriptArray UUIDToMonoArray(const UUID& id);
		static UUID MonoArrayToUUID(ScriptArray uuidArray);

		static MonoString* UTF8ToMonoString(const std::string& str);
		static std::string MonoStringToUTF8(MonoString* monoStr);
	};
}