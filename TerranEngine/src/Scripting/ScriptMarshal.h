#pragma once

#include "ScriptArray.h"

#include "Core/UUID.h"
#include "Utils/Variant.h"

#include <string>

extern "C"
{
	typedef struct _MonoString MonoString;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoArray MonoArray;
}

namespace TerranEngine 
{
	class ScriptMarshal
	{
	public:
		static ScriptArray UUIDToMonoArray(const UUID& id);
		static UUID MonoArrayToUUID(MonoArray* id);

		static MonoString* UTF8ToMonoString(const std::string& str);
		static std::string MonoStringToUTF8(MonoString* monoStr);

		static Utils::Variant MonoObjectToVariant(MonoObject* monoObject, const ScriptType& type);
		static Utils::Variant::Type ScriptTypeToVariantType(const ScriptType& type);
	};
}