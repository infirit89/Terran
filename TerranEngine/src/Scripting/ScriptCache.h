#pragma once

#include "Core/Hash.h"
#include "Core/Base.h"

#include "ScriptClass.h"
#include "ScriptAssembly.h"

#include <unordered_map>
#include <string>

namespace TerranEngine 
{
	#define TR_API_CACHED_CLASS(className) ScriptCache::GetCachedClassFromName("Terran."#className)
	#define TR_CORE_CACHED_CLASS(className) ScriptCache::GetCachedClassFromName("System."#className)

	#define TR_CLASS_ID(className) Hash::FNVHash(className)
	#define TR_API_CLASS_ID(className) TR_CLASS_ID("Terran."#className)
	#define TR_CORLIB_CLASS_ID(className) TR_CLASS_ID("System."#className)

	class ScriptCache 
	{
	public:
		static void CacheCoreClasses();
		static void ClearCache();

		static void GenerateCacheForAssembly(Shared<AssemblyInfo>& assemblyInfo);
		
		static ScriptClass* GetCachedClassFromName(const std::string& className);
		static ScriptClass* GetCachedClassFromID(uint32_t classID);
		
		static ScriptMethod* GetCachedMethod(const std::string& className, const std::string& methodName);

		static ScriptField* GetCachedFieldFromName(const std::string& className, const std::string& fieldName);
		static ScriptField* GetCachedFieldFromID(uint32_t fieldID);
		
		static void CacheClassesFromAssemblyInfo(Shared<AssemblyInfo>& assemblyInfo);
		static void CacheMethodsFromAssemblyInfo(Shared<AssemblyInfo>& assemblyInfo);
		static void CacheFieldsFromAssemblyInfo(Shared<AssemblyInfo>& assemblyInfo);

		static void CacheFields(Shared<ScriptAssembly>& assembly, const ScriptClass& klass);
		static void CacheMethods(Shared<ScriptAssembly>& assembly, const ScriptClass& klass);
		
		static std::unordered_map<uint32_t, ScriptClass> s_CachedClasses;
		static std::unordered_map<uint32_t, std::vector<ScriptMethod>> s_CachedMethods;
		static std::unordered_map<uint32_t, ScriptField> s_CachedFields;
	};
}
