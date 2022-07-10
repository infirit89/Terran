#include "trpch.h"

#include "ScriptEngine.h"
#include "ScriptCache.h"
#include "ScriptClass.h"
#include "ScriptAssembly.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/tokentype.h>

namespace TerranEngine 
{
	#define TR_CLASS(className) ScriptEngine::GetClassFromName("Terran."#className, TR_CORE_ASSEMBLY_INDEX)

	std::unordered_map<uint32_t, ScriptClass> ScriptCache::s_CachedClasses;
	std::unordered_map<uint32_t, std::vector<ScriptMethod>> ScriptCache::s_CachedMethods;
	std::unordered_map<uint32_t, ScriptField> ScriptCache::s_CachedFields;

	void ScriptCache::CacheCoreClasses()
	{
#define CACHE_CORLIB_CLASS(klass) s_CachedClasses.emplace(TR_CORLIB_CLASS_ID(klass), ScriptClass(mono_class_from_name(mono_get_corlib(), "System", #klass))) 
		
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

#define CACHE_API_CLASS(klass)\
	ScriptClass managedClass##klass = TR_CLASS(klass);\
	s_CachedClasses.emplace(TR_API_CLASS_ID(klass), managedClass##klass);\
	CacheFields(ScriptEngine::GetAssembly(TR_CORE_ASSEMBLY_INDEX), managedClass##klass);\
	CacheMethods(ScriptEngine::GetAssembly(TR_CORE_ASSEMBLY_INDEX), managedClass##klass)
		
		// api core classes
		CACHE_API_CLASS(Vector2);
		CACHE_API_CLASS(Vector3);
		CACHE_API_CLASS(Color);
		CACHE_API_CLASS(UUID);
		CACHE_API_CLASS(Entity);
		CACHE_API_CLASS(Component);
		CACHE_API_CLASS(Collider2D);
		CACHE_API_CLASS(Rigidbody2D);
		CACHE_API_CLASS(BoxCollider2D);
		CACHE_API_CLASS(CircleCollider2D);
		CACHE_API_CLASS(Scriptable);
		CACHE_API_CLASS(Tag);
		CACHE_API_CLASS(Transform);
		CACHE_API_CLASS(SpriteRenderer);
		CACHE_API_CLASS(Camera);
		CACHE_API_CLASS(CircleRenderer);
	}

	void ScriptCache::ClearCache()
	{
		s_CachedClasses.clear();
		s_CachedMethods.clear();
		s_CachedFields.clear();
	}

	void ScriptCache::GenerateCacheForAssembly(Shared<AssemblyInfo>& assemblyInfo)
	{
		CacheClassesFromAssemblyInfo(assemblyInfo);
		CacheMethodsFromAssemblyInfo(assemblyInfo);
		CacheFieldsFromAssemblyInfo(assemblyInfo);
	}

	ScriptClass* ScriptCache::GetCachedClassFromName(const std::string& className)
	{
		const uint32_t classID = TR_CLASS_ID(className);
		return GetCachedClassFromID(classID);
	}

	ScriptClass* ScriptCache::GetCachedClassFromID(uint32_t classID)
	{
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

	void ScriptCache::CacheClassesFromAssemblyInfo(Shared<AssemblyInfo>& assemblyInfo)
	{
		for (auto [namespaceName, classNames] : assemblyInfo->ClassInfoMap) 
		{
			for (const std::string& className : classNames)
			{
				std::string formattedModuleName = fmt::format("{0}.{1}", namespaceName, className);
				ScriptClass klass = ScriptEngine::GetClassFromName(formattedModuleName, TR_APP_ASSEMBLY_INDEX);
				
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
				ScriptMethod method = ScriptEngine::GetMethodFromDesc(formattedMethodSignature, TR_APP_ASSEMBLY_INDEX);

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

	void ScriptCache::CacheFields(Shared<ScriptAssembly>& assembly, const ScriptClass& klass)
	{
		const MonoTableInfo* typedefTableInfo = mono_image_get_table_info(assembly->GetMonoImage(), MONO_TABLE_TYPEDEF);
		const MonoTableInfo* fieldTableInfo = mono_image_get_table_info(assembly->GetMonoImage(), MONO_TABLE_FIELD);

		const int fieldTableRows = mono_table_info_get_rows(fieldTableInfo);
		
		if(fieldTableRows <= 0)
			return;
		
		const int typedefTableRows = mono_table_info_get_rows(typedefTableInfo);

		const int klassTypedefIndex = (klass.GetTypeToken() & ~MONO_TOKEN_TYPE_DEF) - 1;
		
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(typedefTableInfo, klassTypedefIndex, cols, MONO_TYPEDEF_SIZE);

		uint32_t nextCol[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(typedefTableInfo, std::min(klassTypedefIndex + 1, typedefTableRows - 1),
								nextCol, MONO_TYPEDEF_SIZE);
		
		for (size_t j = cols[MONO_TYPEDEF_FIELD_LIST] - 1; j
			   < std::min(nextCol[MONO_TYPEDEF_FIELD_LIST], (uint32_t)fieldTableRows) - 1; j++)
		{
			uint32_t fieldCols[MONO_FIELD_SIZE];
			mono_metadata_decode_row(fieldTableInfo, j, fieldCols, MONO_FIELD_SIZE);
			std::string fieldName = mono_metadata_string_heap(assembly->GetMonoImage(), fieldCols[MONO_FIELD_NAME]);
			ScriptField field = klass.GetFieldFromName(fieldName);

			if(field)
			{
				std::string fullFieldName = fmt::format("{0}.{1}", klass.GetFullName(), field.GetName());
				uint32_t fieldID = Hash::FNVHash(fullFieldName);
				field.m_ID = fieldID;
				s_CachedFields.emplace(fieldID, field);
			}
		}
	}

	void ScriptCache::CacheMethods(Shared<ScriptAssembly>& assembly,const ScriptClass& klass)
	{
		const MonoTableInfo* typedefTableInfo = mono_image_get_table_info(assembly->GetMonoImage(), MONO_TABLE_TYPEDEF);
		const MonoTableInfo* methodTableInfo = mono_image_get_table_info(assembly->GetMonoImage(), MONO_TABLE_METHOD);

		const int methodTableRows = mono_table_info_get_rows(methodTableInfo);
		
		if(methodTableRows <= 0)
			return;
		
		const int typedefTableRows = mono_table_info_get_rows(typedefTableInfo);
		const int klassTypedefIndex = (klass.GetTypeToken() & ~MONO_TOKEN_TYPE_DEF) - 1;
		
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(typedefTableInfo, klassTypedefIndex, cols, MONO_TYPEDEF_SIZE);

		uint32_t nextCol[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(typedefTableInfo, std::min(klassTypedefIndex + 1, typedefTableRows - 1),
								nextCol, MONO_TYPEDEF_SIZE);
		
		for (size_t j = cols[MONO_TYPEDEF_METHOD_LIST] - 1; j
			   < std::min(nextCol[MONO_TYPEDEF_METHOD_LIST], (uint32_t)methodTableRows) - 1; j++)
		{
			uint32_t methodCols[MONO_METHOD_SIZE];
			mono_metadata_decode_row(methodTableInfo, j, methodCols, MONO_METHOD_SIZE);
			
			std::string methodName = mono_metadata_string_heap(assembly->GetMonoImage(), methodCols[MONO_METHOD_NAME]);
			const char* blob = mono_metadata_blob_heap(assembly->GetMonoImage(), methodCols[MONO_METHOD_SIGNATURE]);
                
			const char* c;
			uint32_t val = mono_metadata_decode_value(blob, &c);
			const char* cc;
			MonoMethodSignature* signature = mono_metadata_parse_method_signature(assembly->GetMonoImage(), val, c, &cc);
                
			char* methodDesc = mono_signature_get_desc(signature, false);
			std::string formattedMethodDesc = fmt::format(":{0}({1})", methodName, methodDesc);
			std::string moduleName = fmt::format("{0}.{1}", klass.GetNamespace(), klass.GetName());
			mono_metadata_free_method_signature(signature);

			std::string formattedMethodSignature = fmt::format("{0}{1}", moduleName, formattedMethodDesc);
			ScriptMethod method = assembly->GetMethodFromDesc(formattedMethodSignature);

			if (method)
				s_CachedMethods[TR_CLASS_ID(moduleName)].emplace_back(std::move(method));
		}
	}
}
