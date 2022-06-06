#include "trpch.h"

#include "ScriptEngine.h"
#include "ScriptCache.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>

namespace TerranEngine 
{
	#define TR_CLASS(className) ScriptEngine::GetClassFromName("Terran."#className)

	std::unordered_map<uint32_t, ScriptClass*> ScriptCache::s_CachedClasses;
	std::unordered_map<uint32_t, std::vector<ScriptMethod*>> ScriptCache::s_CachedMethods;

	void ScriptCache::CacheCoreClasses()
	{
		// corlib classes
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Byte), new ScriptClass(mono_get_byte_class()));
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Short), new ScriptClass(mono_get_int16_class()));
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Int), new ScriptClass(mono_get_int32_class()));
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Long), new ScriptClass(mono_get_int64_class()));
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Bool), new ScriptClass(mono_get_boolean_class()));
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Float), new ScriptClass(mono_get_single_class()));
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Double), new ScriptClass(mono_get_double_class()));

		// api core classes
		s_CachedClasses.emplace(TR_API_CLASS_ID(Vector2), TR_CLASS(Vector2));
		s_CachedClasses.emplace(TR_API_CLASS_ID(Vector3), TR_CLASS(Vector3));
		s_CachedClasses.emplace(TR_API_CLASS_ID(Entity), TR_CLASS(Entity));
		s_CachedClasses.emplace(TR_API_CLASS_ID(UUID), TR_CLASS(UUID));
		s_CachedClasses.emplace(TR_API_CLASS_ID(Component), TR_CLASS(Component));
		s_CachedClasses.emplace(TR_API_CLASS_ID(Collider2D), TR_CLASS(Collider2D));
		s_CachedClasses.emplace(TR_API_CLASS_ID(Rigidbody2D), TR_CLASS(Rigidbody2D));
		s_CachedClasses.emplace(TR_API_CLASS_ID(BoxCollider2D), TR_CLASS(BoxCollider2D));
		s_CachedClasses.emplace(TR_API_CLASS_ID(CircleCollider2D), TR_CLASS(CircleCollider2D));
		s_CachedClasses.emplace(TR_API_CLASS_ID(Scriptable), TR_CLASS(Scriptable));
		s_CachedClasses.emplace(TR_API_CLASS_ID(Tag), TR_CLASS(Tag));
		s_CachedClasses.emplace(TR_API_CLASS_ID(Transform), TR_CLASS(Transform));
	}

	void ScriptCache::ClearCache()
	{
		for (auto [hashedName, klass] : s_CachedClasses)
			delete klass;

		for (auto [hashedModuleName, methods] : s_CachedMethods)
		{
			for (auto method : methods)
				delete method;

			methods.clear();
		}

		s_CachedClasses.clear();
		s_CachedMethods.clear();
	}

	ScriptClass* ScriptCache::GetCachedClassFromName(const std::string& className)
	{
		uint32_t classID = TR_CLASS_ID(className);

		if(s_CachedClasses.find(classID) != s_CachedClasses.end())
			return s_CachedClasses.at(classID);

		return nullptr;
	}

	ScriptMethod* ScriptCache::GetCachedMethod(const std::string& className, const std::string& methodName)
	{
		uint32_t classID = TR_CLASS_ID(className);

		ScriptMethod* method = nullptr;

		if (s_CachedMethods.find(classID) != s_CachedMethods.end()) 
		{
			// TODO: better way of searching methods
			for (auto cachedMethod : s_CachedMethods.at(classID))
			{
				std::string formattedMethodDesc = fmt::format("{0}{1}", className, methodName);
				MonoMethodDesc* methodDesc = mono_method_desc_new(formattedMethodDesc.c_str(), false);

				if (mono_method_desc_match(methodDesc, cachedMethod->GeMonoMethod()))
					method = cachedMethod;

				mono_method_desc_free(methodDesc);
			}
		}

		return method;
	}

	void ScriptCache::CacheClassesFromAssemblyInfo(Shared<AssemblyInfo>& assemblyInfo)
	{
		for (auto [namespaceName, classNames] : assemblyInfo->ClassInfoMap) 
		{
			for (auto className : classNames)
			{
				std::string formattedModuleName = fmt::format("{0}.{1}", namespaceName, className);
				ScriptClass* klass = ScriptEngine::GetClassFromName(formattedModuleName);
				
				if(klass)
					s_CachedClasses.emplace(TR_CLASS_ID(formattedModuleName), klass);
			}
		}
	}

	void ScriptCache::CacheMethodsFromAssemblyInfo(Shared<AssemblyInfo>& assemblyInfo)
	{
		for (auto[moduleName, methodSignatures] : assemblyInfo->MethodInfoMap)
		{
			for (auto methodSignature : methodSignatures)
			{
				std::string formattedMethodSignature = fmt::format("{0}{1}", moduleName, methodSignature);
				ScriptMethod* method = ScriptEngine::GetMethodFromDesc(formattedMethodSignature);

				if (method)
					s_CachedMethods[TR_CLASS_ID(moduleName)].emplace_back(std::move(method));
			}
		}
	}
}
