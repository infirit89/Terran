#include "trpch.h"
#include "ScriptEngine.h"

#include "Core/Log.h"
#include "Core/FileUtils.h"

#include "ScriptString.h"
#include "ScriptBindings.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Scene/SceneManager.h"

#include <glm/glm.hpp>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/mono-debug.h>
#include <mono/utils/mono-logger.h>

#include <unordered_map>

namespace TerranEngine
{
	static MonoDomain* s_CoreDomain;
	static MonoDomain* s_NewDomain;

	static MonoAssembly* s_CurrentAssembly;
	
	static MonoImage* s_CurrentImage;
	
	static const char* s_AssemblyPath;

	static std::unordered_map<uint32_t, Shared<ScriptClass>> s_Classes;

	static MonoAssembly* LoadAssembly(const char* fileName);
	static MonoImage* GetImageFromAssemly(MonoAssembly* assembly);

	static void OnLogMono(const char* log_domain, const char* log_level, const char* message, mono_bool fatal, void* user_data);

	static Shared<ScriptMethod> GetMethodFromImage(MonoImage* image, const char* methodSignature);

	struct ScriptableInstance 
	{
		Shared<ScriptObject> Object;
		Shared<ScriptMethod> Constructor;
		Shared<ScriptMethod> InitMethod;
		Shared<ScriptMethod> UpdateMethod;

		void GetMethods(const Shared<ScriptClass>& scriptClass) 
		{
			Constructor = GetMethodFromImage(s_CurrentImage, "TerranScriptCore.Scriptable:.ctor(uint)");

			InitMethod = scriptClass->GetMethod(":Init()");
			UpdateMethod = scriptClass->GetMethod(":Update()");
		}
	};

	static std::unordered_map<UUID, ScriptableInstance> s_ScriptableInstanceMap;

	static ScriptableInstance GetInstance(const UUID& uuid) 
	{
		if (s_ScriptableInstanceMap.find(uuid) != s_ScriptableInstanceMap.end())
			return s_ScriptableInstanceMap[uuid];


		return { };
	}

	void ScriptEngine::Init(const char* fileName)
	{
		s_AssemblyPath = fileName;
		std::string monoPath = FileUtils::GetEnvironmentVariable("MONO_PATH");
		
		std::string libPath = monoPath + "lib";
		std::string etcPath = monoPath + "etc";

		mono_set_dirs(libPath.c_str(), etcPath.c_str());

#ifdef TR_DEBUG

		mono_debug_init(MONO_DEBUG_FORMAT_MONO);

		mono_trace_set_level_string("debug");

		mono_trace_set_log_handler(OnLogMono, nullptr);

#endif

		s_CoreDomain = mono_jit_init("CoreDomain");
		
		if (!s_CoreDomain)
		{
			TR_ERROR("Couldn't initialize the Mono Domain!");
			return;
		}

		NewDomain();
	}

	void ScriptEngine::CleanUp()
	{
		mono_jit_cleanup(s_CoreDomain);
		TR_INFO("Deinitialized the scripting core");
	}

	void ScriptEngine::NewDomain() 
	{
		s_NewDomain = mono_domain_create_appdomain("ScriptAssemblyDomain", NULL);
		
		if (!mono_domain_set(s_NewDomain, false)) 
		{
			TR_ERROR("Couldn't set the new domain");
			return;
		}

		s_CurrentAssembly = LoadAssembly(s_AssemblyPath);

		if (!s_CurrentAssembly) return;

		s_CurrentImage = GetImageFromAssemly(s_CurrentAssembly);

		if (!s_CurrentImage) return;

		TR_INFO("Successfuly loaded: {0}", s_AssemblyPath);

		ScriptBindings::Bind();
	}

	std::string ScriptEngine::GetAssemblyPath() 
	{
		return s_AssemblyPath;
	}

	void ScriptEngine::UnloadDomain() 
	{
		if (s_NewDomain != s_CoreDomain) 
		{
			mono_domain_set(s_CoreDomain, false);

			if (!mono_domain_finalize(s_NewDomain, 2000))
			{
				TR_ERROR("Finalizing the domain timed out");
				return;
			}
			mono_gc_collect(mono_gc_max_generation());

			mono_domain_unload(s_NewDomain);

			s_Classes.clear();
		}
	}

	Shared<ScriptClass> ScriptEngine::GetClass(const std::string& moduleName)
	{
		if (!s_CurrentImage)
		{
			TR_ERROR("Can't locate the class {0}, as there is no loaded script image", moduleName);
			return NULL;
		}

		std::hash<std::string> hasher;
		uint32_t hashedName = hasher(moduleName);

		if (s_Classes.find(hashedName) != s_Classes.end())
			return s_Classes[hashedName];
		
		size_t dotPosition = moduleName.find_last_of(".");

		std::string& namespaceName = moduleName.substr(0, dotPosition);
		std::string& className = moduleName.substr(dotPosition + 1);
		
		// NOTE: Access violation when the script assembly isn't loaded
		MonoClass* klass = mono_class_from_name(s_CurrentImage, namespaceName.c_str(), className.c_str());

		if (!klass) 
		{
			TR_ERROR("Class wasn't found");
			return NULL;
		}
		Shared<ScriptClass> scriptClass = CreateShared<ScriptClass>(klass);
		s_Classes[hashedName] = scriptClass;

		return scriptClass;
	}


	static Shared<ScriptMethod> GetMethodFromImage(MonoImage* image, const char* methodSignature)
	{
		MonoMethodDesc* monoDesc = mono_method_desc_new(methodSignature, false);
		if (!monoDesc)
		{
			TR_ERROR("Couldn't find a matching description ({0}) in the image", methodSignature);

			return NULL;
		}

		MonoMethod* monoMethod = mono_method_desc_search_in_image(monoDesc, image);

		if (!monoMethod)
		{
			TR_ERROR("Couldn't find the method with signature: {0} in image", methodSignature);

			return NULL;
		}

		Shared<ScriptMethod> method = CreateShared<ScriptMethod>(monoMethod);

		mono_method_desc_free(monoDesc);

		return method;
	}

	void ScriptEngine::InitializeEntity(Entity entity)
	{
		ScriptComponent& scriptComponent = entity.GetComponent<ScriptComponent>();
		Shared<ScriptClass> klass = ScriptEngine::GetClass(scriptComponent.ModuleName);
		if (!klass) 
		{
			TR_ERROR("Couldn't find the class: {0}", scriptComponent.ModuleName);
			return;
		}

		ScriptableInstance instance;
		instance.Object = klass->CreateInstance();
		instance.GetMethods(klass);

		uint32_t entityID = entity;
		void* args[] = { &entityID };

		instance.Constructor->Invoke(instance.Object, args);

		s_ScriptableInstanceMap[entity.GetID()] = instance;
	}

	void ScriptEngine::StartScriptable(Entity entity) 
	{
		ScriptableInstance instace = GetInstance(entity.GetID());

		if (instace.InitMethod)
			instace.InitMethod->Invoke(instace.Object, nullptr);
	}

	void ScriptEngine::UpdateScriptable(Entity entity) 
	{
		ScriptableInstance instance = GetInstance(entity.GetID());

		if (instance.UpdateMethod)
			instance.UpdateMethod->Invoke(instance.Object, nullptr);
	}

	static MonoAssembly* LoadAssembly(const char* fileName) 
	{
		MonoAssembly* newAssembly = mono_domain_assembly_open(s_NewDomain, fileName);

		if (!newAssembly)
		{
			TR_ERROR("Couldn't load the C# assembly!");
			return nullptr;
		}

		return newAssembly;
	}

	static MonoImage* GetImageFromAssemly(MonoAssembly* assembly)
	{
		MonoImage* newImage = mono_assembly_get_image(assembly);

		if (!newImage)
		{
			TR_ERROR("Couldn't find an image in the script assembly!");
			return nullptr;
		}

		return newImage;
	}

	static void OnLogMono(const char* log_domain, const char* log_level, const char* message, mono_bool fatal, void* user_data) 
	{
		if (log_level != nullptr) 
		{
			if (strcmp(log_level, "info") == 0)
				TR_INFO("Domain: {0}; Message: {1}", log_domain, message);
			else if(strcmp(log_level, "debug") == 0)
				TR_TRACE("Domain: {0}; Message: {1}", log_domain, message);
			else
				TR_TRACE("Domain: {0}; Message: {1}; Log Level: {2}", log_domain, message, log_level);
		}
	}
}
