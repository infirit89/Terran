#include "trpch.h"

#include "ScriptEngine.h"
#include "ScriptCache.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>

namespace TerranEngine 
{
	#define TR_CLASS(className) ScriptEngine::GetClassFromName("Terran."#className)

	std::unordered_map<uint32_t, ScriptClass> ScriptCache::s_CachedClasses;
	std::unordered_map<uint32_t, std::vector<ScriptMethod>> ScriptCache::s_CachedMethods;
	std::unordered_map<uint32_t, ScriptField> ScriptCache::s_CachedFields;

	void ScriptCache::CacheCoreClasses()
	{
		// corlib classes
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Byte), ScriptClass(mono_get_byte_class()));
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Short), ScriptClass(mono_get_int16_class()));
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Int),  ScriptClass(mono_get_int32_class()));
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Long), ScriptClass(mono_get_int64_class()));
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Bool), ScriptClass(mono_get_boolean_class()));
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Float), ScriptClass(mono_get_single_class()));
		s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(Double), ScriptClass(mono_get_double_class()));

#define CACHE_API_CLASS(klass) s_CachedClasses.emplace(TR_API_CLASS_ID(klass), TR_CLASS(klass))
		// api core classes
		CACHE_API_CLASS(Vector2);
		CACHE_API_CLASS(Vector3);
		CACHE_API_CLASS(Entity);
		CACHE_API_CLASS(UUID);
		CACHE_API_CLASS(Component);
		CACHE_API_CLASS(Collider2D);
		CACHE_API_CLASS(Rigidbody2D);
		CACHE_API_CLASS(BoxCollider2D);
		CACHE_API_CLASS(CircleCollider2D);
		CACHE_API_CLASS(Scriptable);
		CACHE_API_CLASS(Tag);
		CACHE_API_CLASS(Transform);
		CACHE_API_CLASS(Color);
	}

	void ScriptCache::ClearCache()
	{
		s_CachedClasses.clear();
		s_CachedMethods.clear();
	}

	ScriptClass* ScriptCache::GetCachedClassFromName(const std::string& className)
	{
		const uint32_t classID = TR_CLASS_ID(className);

		if(s_CachedClasses.find(classID) != s_CachedClasses.end())
			return &s_CachedClasses.at(classID);

		return nullptr;
	}

	ScriptMethod* ScriptCache::GetCachedMethod(const std::string& className, const std::string& methodName)
	{
		const uint32_t classID = TR_CLASS_ID(className);

		ScriptMethod* method = nullptr;

		if (s_CachedMethods.find(classID) != s_CachedMethods.end()) 
		{
			std::vector<ScriptMethod>& methods = s_CachedMethods.at(classID);
			const auto it = std::find_if(methods.begin(), methods.end(),
	     [&](ScriptMethod method)
	     {
	         const std::string formattedMethodDesc = fmt::format("{0}{1}", className, methodName);
	         MonoMethodDesc* methodDesc = mono_method_desc_new(formattedMethodDesc.c_str(), false);

	         if (mono_method_desc_match(methodDesc, method.GetMonoMethod()))
	             return true;

	         mono_method_desc_free(methodDesc);
	         return false;
	     });
			
			if(it != methods.end())
				method = &methods.at(it - methods.begin());	
		}

		return method;
	}

	ScriptField* ScriptCache::GetCachedFieldFromName(const std::string& className, const std::string& fieldName)
	{
		const std::string fullFieldName = fmt::format("{0}.{1}", className, fieldName);
		const uint32_t fieldID = Hash::FNVHash(fullFieldName);

		if(s_CachedFields.find(fieldID) != s_CachedFields.end())
			return &s_CachedFields.at(fieldID);

		return nullptr;
	}

	ScriptField* ScriptCache::GetCachedFieldFromID(uint32_t fieldID)
	{
		if(s_CachedFields.find(fieldID) != s_CachedFields.end())
			return &s_CachedFields.at(fieldID);
		
		return nullptr;
	}

	void ScriptCache::CacheClassesFromAssemblyInfo(Shared<AssemblyInfo>& assemblyInfo)
	{
		for (auto [namespaceName, classNames] : assemblyInfo->ClassInfoMap) 
		{
			for (const std::string& className : classNames)
			{
				std::string formattedModuleName = fmt::format("{0}.{1}", namespaceName, className);
				ScriptClass klass = ScriptEngine::GetClassFromName(formattedModuleName);
				
				if(klass)
					s_CachedClasses.emplace(TR_CLASS_ID(formattedModuleName), klass);
			}
		}
	}

	void ScriptCache::CacheMethodsFromAssemblyInfo(Shared<AssemblyInfo>& assemblyInfo)
	{
		for (auto[moduleName, methodSignatures] : assemblyInfo->MethodInfoMap)
		{
			for (const std::string& methodSignature : methodSignatures)
			{
				std::string formattedMethodSignature = fmt::format("{0}{1}", moduleName, methodSignature);
				ScriptMethod method = ScriptEngine::GetMethodFromDesc(formattedMethodSignature);

				if (method)
					s_CachedMethods[TR_CLASS_ID(moduleName)].emplace_back(std::move(method));
			}
		}
	}

	void ScriptCache::CacheFieldsFromAssemblyInfo(Shared<AssemblyInfo>& assemblyInfo)
	{
		for (auto [moduleName, fieldNames] : assemblyInfo->FieldInfoMap)
		{
			for (const std::string& fieldName : fieldNames)
			{
				ScriptClass* klass = GetCachedClassFromName(moduleName);
				
				if(!klass)
					continue;
				
				ScriptField field = klass->GetFieldFromName(fieldName);

				if(field)
				{
					std::string fullFieldName = fmt::format("{0}.{1}", moduleName, field.GetName());
					uint32_t fieldID = Hash::FNVHash(fullFieldName);
					field.m_ID = fieldID;
					s_CachedFields.emplace(fieldID, std::move(field));
					klass->m_Fields.emplace_back(field);
				}
			}
		}
	}
}
