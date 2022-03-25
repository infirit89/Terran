#pragma once

#include "Core/UUID.h"
#include <mono/metadata/object.h>

namespace TerranEngine 
{
	class ScriptMarshal
	{
	public:
		static MonoArray* UUIDToMonoArray(const UUID& id);
		static UUID MonoArrayToUUID(MonoArray* uuidArray);
	};
}