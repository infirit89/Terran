#include "trpch.h"

#include "ScriptEngine.h"
#include "ScriptCache.h"
#include "ManagedClass.h"
#include "ScriptAssembly.h"
#include "ManagedMetadata.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/tokentype.h>

namespace TerranEngine 
{
	#define TR_CLASS(className) ScriptEngine::GetClassFromName("Terran."#className, TR_CORE_ASSEMBLY_INDEX)

	static std::unordered_map<uint32_t, ManagedClass> s_CachedClasses;
	static std::unordered_map<uint32_t, std::vector<ManagedMethod>> s_CachedMethods;
	static std::unordered_map<uint32_t, ScriptField> s_CachedFields;

	void ScriptCache::CacheCoreClasses()
	{
#define CACHE_CORLIB_CLASS(klass) s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(klass), ManagedClass(mono_class_from_name(mono_get_corlib(), "System", #klass)))
		
		// corlib classes
		CACHE_CORLIB_CLASS(Byte);
		CACHE_CORLIB_CLASS(UInt16);
		CACHE_CORLIB_CLASS(UInt32);
		CACHE_CORLIB_CLASS(UInt64);
		
		CACHE_CORLIB_CLASS(SByte);
		CACHE_CORLIB_CLASS(Int16);
		CACHE_CORLIB_CLASS(Int32);
		CACHE_CORLIB_CLASS(Int64);

		CACHE_CORLIB_CLASS(Boolean);

		CACHE_CORLIB_CLASS(Single);
		CACHE_CORLIB_CLASS(Double);

		CACHE_CORLIB_CLASS(Object);

		CACHE_CORLIB_CLASS(String);

#define CACHE_API_CLASS(klass)								\
	ManagedClass managedClass##klass = TR_CLASS(klass);		\
	uint32_t klass##ID = TR_API_CLASS_ID(klass);			\
	s_CachedClasses.emplace(klass##ID, managedClass##klass);\
	CacheFieldsForClass(s_CachedClasses.at(klass##ID));		\
	CacheMethodsForClass(s_CachedClasses.at(klass##ID))
		
		// api core classes
		/*CACHE_API_CLASS(Vector2);
		CACHE_API_CLASS(Vector3);
		CACHE_API_CLASS(LayerMask);
		CACHE_API_CLASS(Color);
		CACHE_API_CLASS(UUID);
		CACHE_API_CLASS(Entity);
		CACHE_API_CLASS(Component);
		CACHE_API_CLASS(Collider2D);
		CACHE_API_CLASS(Rigidbody2D);
		CACHE_API_CLASS(BoxCollider2D);
		CACHE_API_CLASS(CircleCollider2D);
		CACHE_API_CLASS(CapsuleCollider2D);
		CACHE_API_CLASS(Scriptable);
		CACHE_API_CLASS(Tag);
		CACHE_API_CLASS(Transform);
		CACHE_API_CLASS(SpriteRenderer);
		CACHE_API_CLASS(Camera);
		CACHE_API_CLASS(CircleRenderer);
		CACHE_API_CLASS(TextRenderer);
		CACHE_API_CLASS(RayCastHitInfo2D);
		CACHE_API_CLASS(KeyCode);*/
	}

	void ScriptCache::ClearCache()
	{
		s_CachedClasses.clear();
		s_CachedMethods.clear();
		s_CachedFields.clear();
	}

	void ScriptCache::GenerateCacheForAssembly(Shared<ScriptAssembly> assembly)
	{
		std::vector<ManagedClass> classList = ManagedMetadata::GetClassListFromMetadata(assembly);
		for(ManagedClass klass : classList) 
		{
			CacheFieldsForClass(klass);
			CacheMethodsForClass(klass);
			s_CachedClasses.emplace(TR_CLASS_ID(klass.GetFullName()), std::move(klass));
		}
	}

	ManagedClass* ScriptCache::GetCachedClassFromName(std::string_view className)
	{
		const uint32_t classID = TR_CLASS_ID(className);
		return GetCachedClassFromID(classID);
	}

	ManagedClass* ScriptCache::GetCachedClassFromID(uint32_t classID)
	{
		if(s_CachedClasses.find(classID) != s_CachedClasses.end())
			return &s_CachedClasses.at(classID);
		
		return nullptr;
	}

	ManagedMethod* ScriptCache::GetCachedMethod(std::string_view className, std::string_view methodName)
	{
		const uint32_t classID = TR_CLASS_ID(className);

		ManagedMethod* method = nullptr;

		if (s_CachedMethods.find(classID) != s_CachedMethods.end()) 
		{
			std::vector<ManagedMethod>& methods = s_CachedMethods.at(classID);
			const auto it = std::find_if(methods.begin(), methods.end(),
			[&](ManagedMethod method)
			     {
					 bool matchedMethodDesc = false;
			         const std::string formattedMethodDesc = fmt::format("{0}{1}", className, methodName);
			         MonoMethodDesc* methodDesc = mono_method_desc_new(formattedMethodDesc.c_str(), false);

			         if (mono_method_desc_match(methodDesc, method.GetMonoMethod()))
			             matchedMethodDesc = true;

			         mono_method_desc_free(methodDesc);
			         return matchedMethodDesc;
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

	void ScriptCache::CacheFieldsForClass(ManagedClass& klass)
	{
		void* iter = nullptr;

		while(true) 
		{
			MonoClassField* monoField = mono_class_get_fields(klass, &iter);

			if (monoField)
			{
				ScriptField field = monoField;
				std::string fullFieldName = fmt::format("{0}.{1}", klass.GetFullName(), field.GetName());
				uint32_t fieldID = Hash::FNVHash(fullFieldName);
				field.m_ID = fieldID;
				s_CachedFields.emplace(fieldID, field);
				klass.m_Fields.push_back(field);
			}
			else
				break;
		}
	}

	void ScriptCache::CacheMethodsForClass(const ManagedClass& klass)
	{
		for (const ManagedMethod& method : klass.GetMethods())
			s_CachedMethods[TR_CLASS_ID(klass.GetFullName())].emplace_back(method);
	}
}
