#pragma once

#include "Core/Hash.h"
#include "Core/Base.h"

#include "ManagedClass.h"
#include "ScriptAssembly.h"

#include <unordered_map>
#include <string>

namespace TerranEngine 
{
	#define TR_API_CACHED_CLASS(className) ScriptCache::GetCachedClassFromName("Terran."#className)
	#define TR_CORE_CACHED_CLASS(className) ScriptCache::GetCachedClassFromName("System."#className)

	#define TR_CLASS_ID(className) Hash::FNVHash(className)
	#define TR_API_CLASS_ID(className) TR_CLASS_ID(std::string_view("Terran."#className))
	#define TR_CORLIB_CLASS_ID(className) TR_CLASS_ID(std::string_view("System."#className))

	class ScriptCache 
	{
	public:
		static void CacheCoreClasses();
		static void ClearCache();

		static void GenerateCacheForAssembly(Shared<ScriptAssembly> assembly);
		
		static ManagedClass* GetCachedClassFromName(std::string_view className);
		static ManagedClass* GetCachedClassFromID(uint32_t classID);
		
		static ManagedMethod* GetCachedMethod(std::string_view className, std::string_view methodName);

		static ScriptField* GetCachedFieldFromName(const std::string& className, const std::string& fieldName);
		static ScriptField* GetCachedFieldFromID(uint32_t fieldID);
		
		static void CacheFieldsForClass(ManagedClass& klass);
		static void CacheMethodsForClass(const ManagedClass& klass);
	};
}
