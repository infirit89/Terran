#pragma once
#include "Core/Hash.h"

#include "ScriptClass.h"

#include <unordered_map>
#include <string>

namespace TerranEngine 
{
	#define TR_API_CACHED_CLASS(className) ScriptCache::GetCachedClassFromName("Terran."#className)
	#define TR_CORE_CACHED_CLASS(className) ScriptCache::GetCachedClassFromName("System."#className)
	#define TR_CLASS_ID(className) Hash::FNVHash("Terran."#className)

	class ScriptCache 
	{
	public:
		static void CacheCoreClasses();
		static void ClearClassCache();

		static ScriptClass* GetCachedClassFromName(std::string className);

		static std::unordered_map<uint32_t, ScriptClass> s_CachedClasses;
	};

}
