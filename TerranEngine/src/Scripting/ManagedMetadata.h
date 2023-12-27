#pragma once

#include "Core/Base.h"

#include "ScriptType.h"
#include "ScriptAssembly.h"
#include "ManagedClass.h"

#include <unordered_map>
#include <string>
#include <unordered_set>

namespace TerranEngine 
{
	class ManagedMetadata
	{
	public:
		static std::vector<ManagedClass> GetClassListFromMetadata(Shared<ScriptAssembly> assembly);
		static uint32_t GetMethodCountForClass(Shared<ScriptAssembly> assembly, const ManagedClass* klass);
	};
}
