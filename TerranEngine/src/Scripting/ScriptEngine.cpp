#include "trpch.h"
#include "ScriptEngine.h"

#include "ScriptBindings.h"
#include "ScriptMarshal.h"
#include "GCManager.h"
#include "ScriptCache.h"
#include "ScriptMethodThunks.h"
#include "ScriptAssembly.h"
#include "ScriptObject.h"
#include "ScriptArray.h"

#include "Core/Log.h"
#include "Core/FileUtils.h"
#include "Core/Project.h"

#include "Scene/Components.h"

#include "Utils/Utils.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/mono-debug.h>
#include <mono/utils/mono-logger.h>
#include <mono/metadata/mono-config.h>

#include <unordered_map>

namespace TerranEngine
{
	struct ScriptableInstance 
	{
		GCHandle ObjectHandle;
		ScriptMethodThunks<MonoArray*> Constructor;
		ScriptMethodThunks<> InitMethod;
		ScriptMethodThunks<> UpdateMethod;
		
		ScriptMethodThunks<MonoArray*> PhysicsBeginContact;
		ScriptMethodThunks<MonoArray*> PhysicsEndContact;

		ScriptMethodThunks<> PhysicsUpdateMethod;

		void GetMethods() 
		{
			Constructor.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":.ctor(byte[])"));

			InitMethod.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":Init()"));
			UpdateMethod.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":Update()"));

			PhysicsBeginContact.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":OnCollisionBegin(Entity)"));
			PhysicsEndContact.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":OnCollisionEnd(Entity)"));

			PhysicsUpdateMethod.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":PhysicsUpdate()"));
		}
	};
	
	using ScriptInstanceMap = std::unordered_map<UUID, std::unordered_map<UUID, ScriptableInstance>>;
	
	struct ScriptEngineData 
	{
		MonoDomain* CoreDomain;
		MonoDomain* AppDomain;

		std::vector<Shared<ScriptAssembly>> Assemblies;
		
		std::filesystem::path MonoPath = "mono";

		std::filesystem::path LibPath = MonoPath / "lib";
		std::filesystem::path EtcPath = MonoPath / "etc";

		std::filesystem::path MonoConfigPath = EtcPath / "config";
		Shared<Scene> SceneContext;
		ScriptInstanceMap ScriptInstanceMap;
	};

	static void OnLogMono(const char* logDomain, const char* logLevel, const char* message, mono_bool fatal, void* userData);
	static void UnhandledExceptionHook(MonoObject* exc, void *user_data);
	
	static ScriptEngineData* s_Data;

	static ScriptableInstance s_EmptyInstance;

	static ScriptableInstance& GetInstance(const UUID& sceneUUID, const UUID& entityUUID) 
	{
		if (s_Data->ScriptInstanceMap.find(sceneUUID) != s_Data->ScriptInstanceMap.end()) 
		{
			if (s_Data->ScriptInstanceMap[sceneUUID].find(entityUUID) != s_Data->ScriptInstanceMap[sceneUUID].end())
				return s_Data->ScriptInstanceMap[sceneUUID][entityUUID];
		}

		return s_EmptyInstance;
	}


	void ScriptEngine::Initialize()
	{
		s_Data = new ScriptEngineData;
		
		mono_set_dirs(s_Data->LibPath.string().c_str(), s_Data->EtcPath.string().c_str());

#ifdef TR_DEBUG
		mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		mono_trace_set_level_string("debug");
		mono_trace_set_log_handler(OnLogMono, nullptr);
#endif

		mono_config_parse(s_Data->MonoConfigPath.string().c_str());

		s_Data->CoreDomain = mono_jit_init("CoreDomain");
		
		if (!s_Data->CoreDomain)
		{
			TR_ERROR("Couldn't initialize the Mono Domain!");
			return;
		}

		s_Data->Assemblies.reserve(TR_ASSEMBLIES);
		for (size_t i = 0; i < TR_ASSEMBLIES; i++)
			s_Data->Assemblies.emplace_back(CreateShared<ScriptAssembly>());
		
		mono_install_unhandled_exception_hook(UnhandledExceptionHook, nullptr);		
		
		CreateAppDomain();

		LoadCoreAssembly();
		LoadAppAssembly();
		
		ScriptBindings::Bind();
	}

	void ScriptEngine::Shutdown()
	{
		for (const auto& [sceneID, scriptInstances] : s_Data->ScriptInstanceMap)
		{
			for (const auto& [entityID, instance] : scriptInstances)
			{
				GCHandle& handle = s_Data->ScriptInstanceMap[sceneID][entityID].ObjectHandle;
				GCManager::FreeHandle(handle);
			}
			s_Data->ScriptInstanceMap[sceneID].clear();
		}

		s_Data->ScriptInstanceMap.clear();

		GCManager::CollectAll();
		
		mono_jit_cleanup(s_Data->CoreDomain);
		TR_INFO("Deinitialized the scripting core");
	}

	void ScriptEngine::ReloadAppAssembly()
	{
		std::unordered_map<UUID, std::unordered_map<UUID, std::unordered_map<uint32_t, Utils::Variant>>> scriptFieldsStates;

		for (const auto& [sceneID, scriptInstances] : s_Data->ScriptInstanceMap)
		{
			for (const auto& [entityID, instance] : scriptInstances)
			{
				const GCHandle& handle = instance.ObjectHandle;
				
				auto scene = Scene::GetScene(sceneID);

				Entity entity = scene->FindEntityWithUUID(entityID);

				auto& scriptComponent = entity.GetComponent<ScriptComponent>();
				for (const auto& fieldID : scriptComponent.PublicFieldIDs)
				{
					ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);
					scriptFieldsStates[sceneID][entityID][fieldID] = field->GetData<Utils::Variant>(handle);
				}
			}
		}
		
		UnloadDomain();
		CreateAppDomain();

		LoadCoreAssembly();
		LoadAppAssembly();
		
		for (const auto& [sceneID, scriptFieldsStates] : scriptFieldsStates)
		{
			auto scene = Scene::GetScene(sceneID);
			for (const auto& [entityID, fieldsState] : scriptFieldsStates)
			{
				Entity entity = scene->FindEntityWithUUID(entityID);
				InitializeScriptable(entity);
				
				for (const auto& [fieldID, fieldData] : fieldsState)
				{
					GCHandle& handle = GetScriptInstanceGCHandle(sceneID, entityID);

					if(!handle.IsValid())
						break;

					ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);

					if(!field)
						continue;

					field->SetData<Utils::Variant>(fieldData, handle);
				}
			}
		}
	}

	void ScriptEngine::SetContext(const Shared<Scene>& context) { s_Data->SceneContext = context; }
	Shared<Scene>& ScriptEngine::GetContext() { return s_Data->SceneContext; }
	
	void ScriptEngine::LoadCoreAssembly()
	{
		auto& coreAssembly = s_Data->Assemblies.at(TR_CORE_ASSEMBLY_INDEX);
		coreAssembly = ScriptAssembly::LoadAssembly(Project::GetCoreAssemblyPath());
		ScriptCache::CacheCoreClasses();
	}

	void ScriptEngine::LoadAppAssembly()
	{
		auto& appAssembly = s_Data->Assemblies.at(TR_APP_ASSEMBLY_INDEX);
		appAssembly = ScriptAssembly::LoadAssembly(Project::GetAppAssemblyPath());

		Shared<AssemblyInfo> assemblyInfo = appAssembly->GenerateAssemblyInfo();
		ScriptCache::GenerateCacheForAssembly(assemblyInfo);
	}

	void ScriptEngine::CreateAppDomain() 
	{
		s_Data->AppDomain = mono_domain_create_appdomain("ScriptAssemblyDomain", NULL);
		
		if (!mono_domain_set(s_Data->AppDomain, false))
			TR_ERROR("Couldn't set the new domain");
	}

	void ScriptEngine::UnloadDomain() 
	{
		if (s_Data->AppDomain != s_Data->CoreDomain)
		{
			mono_domain_set(s_Data->CoreDomain, false);

			if (!mono_domain_finalize(s_Data->AppDomain, 2000))
			{
				TR_ERROR("Finalizing the domain timed out");
				return;
			}
			
			GCManager::CollectAll();

			ScriptInstanceMap oldInstanceMap = s_Data->ScriptInstanceMap;
			for (const auto& [sceneID, scriptInstances] : oldInstanceMap)
			{
				for (const auto& [entityID, instance] : scriptInstances)
				{
					auto scene = Scene::GetScene(sceneID);

					if(!scene)
						continue;

					Entity entity = scene->FindEntityWithUUID(entityID);
					UninitalizeScriptable(entity);
				}
			}
			
			mono_domain_unload(s_Data->AppDomain);
		}

		ScriptCache::ClearCache();
	}

	ScriptClass ScriptEngine::GetClassFromName(const std::string& moduleName, int assemblyIndex)
	{
		return s_Data->Assemblies[assemblyIndex]->GetClassFromName(moduleName);
	}

	ScriptClass ScriptEngine::GetClassFromTypeToken(uint32_t typeToken, int assemblyIndex)
	{
		return s_Data->Assemblies[assemblyIndex]->GetClassFromTypeToken(typeToken);
	}

	ScriptMethod ScriptEngine::GetMethodFromDesc(const std::string& methodDesc, int assemblyIndex)
	{
		return s_Data->Assemblies[assemblyIndex]->GetMethodFromDesc(methodDesc);
	}

	bool ScriptEngine::ClassExists(const std::string& moduleName)
	{
		return ScriptCache::GetCachedClassFromName(moduleName);
	}

	Shared<ScriptAssembly>& ScriptEngine::GetAssembly(int assemblyIndex)
	{
		return s_Data->Assemblies.at(assemblyIndex);
	}

	void ScriptEngine::InitializeScriptable(Entity entity)
	{
		if (s_Data->ScriptInstanceMap.find(entity.GetID()) == s_Data->ScriptInstanceMap.end())
		{
			auto& scriptComponent = entity.GetComponent<ScriptComponent>();
			ScriptClass* klass = ScriptCache::GetCachedClassFromName(scriptComponent.ModuleName);
			
			if (!klass) return;

			if (klass->IsInstanceOf(TR_API_CACHED_CLASS(Scriptable)))
			{
				// TODO: display error in ui
				TR_WARN("Class {0} doesn't extend Scriptable", scriptComponent.ModuleName);
				return;
			}

			ScriptableInstance instance;
			const ScriptObject object = ScriptObject::CreateInstace(*klass);
			instance.ObjectHandle = GCManager::CreateStrongHadle(object);
			instance.GetMethods();

			ScriptArray uuidArray = ScriptMarshal::UUIDToMonoArray(entity.GetID());
			
			MonoException* exc = nullptr;
			instance.Constructor.Invoke(object.GetMonoObject(), uuidArray.GetMonoArray(), &exc);
			
			s_Data->ScriptInstanceMap[entity.GetSceneID()][entity.GetID()] = instance;

			scriptComponent.PublicFieldIDs.clear();
			for (auto& field : klass->GetFields())
			{
				if(field.GetVisibility() == ScriptFieldVisibility::Public)
					scriptComponent.PublicFieldIDs.emplace_back(field.GetID());
			}
		}
	}

	void ScriptEngine::UninitalizeScriptable(Entity entity)
	{
		if (!entity || !entity.HasComponent<TagComponent>())
			return;
		
		if (s_Data->ScriptInstanceMap.find(entity.GetSceneID()) != s_Data->ScriptInstanceMap.end()) 
		{
			if (s_Data->ScriptInstanceMap[entity.GetSceneID()].find(entity.GetID()) != s_Data->ScriptInstanceMap[entity.GetSceneID()].end()) 
			{
				GCHandle& handle = s_Data->ScriptInstanceMap[entity.GetSceneID()][entity.GetID()].ObjectHandle;
				GCManager::FreeHandle(handle);
				s_Data->ScriptInstanceMap[entity.GetSceneID()].erase(entity.GetID());
			}

			if(s_Data->ScriptInstanceMap.empty())
				s_Data->ScriptInstanceMap.erase(entity.GetSceneID());
		}
	}

	void ScriptEngine::OnStart(Entity entity) 
	{
		ScriptableInstance instance = GetInstance(entity.GetSceneID(), entity.GetID());

		entity.GetComponent<ScriptComponent>().Started = true;

		if (instance.InitMethod)
		{
			MonoException* exc = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.InitMethod.Invoke(monoObject, &exc);
		}
	}

	void ScriptEngine::OnUpdate(Entity entity) 
	{
		ScriptableInstance instance = GetInstance(entity.GetSceneID(), entity.GetID());

		if (instance.UpdateMethod) 
		{
			MonoException* exc = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.UpdateMethod.Invoke(monoObject, &exc);
		}
	}

	void ScriptEngine::OnPhysicsBeginContact(Entity collider, Entity collidee)
	{
		ScriptableInstance instance = GetInstance(collider.GetSceneID(), collider.GetID());

		if (instance.PhysicsBeginContact) 
		{
			ScriptArray uuidArr = ScriptMarshal::UUIDToMonoArray(collidee.GetID());
			MonoException* exc = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.PhysicsBeginContact.Invoke(monoObject, uuidArr.GetMonoArray(), &exc);
		}
	}

	void ScriptEngine::OnPhysicsEndContact(Entity collider, Entity collidee)
	{
		ScriptableInstance instance = GetInstance(collider.GetSceneID(), collider.GetID());

		if (instance.PhysicsEndContact)
		{
			ScriptArray uuidArr = ScriptMarshal::UUIDToMonoArray(collidee.GetID());
			MonoException* exc = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.PhysicsBeginContact.Invoke(monoObject, uuidArr.GetMonoArray(), &exc);
		}
	}

	void ScriptEngine::OnPhysicsUpdate(Entity entity)
	{
		ScriptableInstance instance = GetInstance(entity.GetSceneID(), entity.GetID());

		if (instance.PhysicsUpdateMethod) 
		{
			MonoException* exc = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.PhysicsUpdateMethod.Invoke(monoObject, &exc);
		}
	}

	ScriptObject ScriptEngine::GetScriptInstanceScriptObject(const UUID& sceneUUID, const UUID& entityUUID)
	{
		const ScriptableInstance instance = GetInstance(sceneUUID, entityUUID);
		return GCManager::GetManagedObject(instance.ObjectHandle);
	}

	GCHandle ScriptEngine::GetScriptInstanceGCHandle(const UUID& sceneUUID, const UUID& entityUUID)
	{
		const ScriptableInstance instance = GetInstance(sceneUUID, entityUUID);
		return instance.ObjectHandle;
	}

	// void ScriptEngine::ClearFieldBackupMap()
	// {
	// 	auto scriptFieldBackupCpy = s_ScriptFieldBackup;
	// 	for (auto& [entityID, fieldBackupMap] : scriptFieldBackupCpy)
	// 	{
	// 		auto fieldBackupCpy = fieldBackupMap;
	// 		for (auto& [fieldID, fieldData] : fieldBackupCpy)
	// 		{
	// 			fieldData.Clear();
	// 			fieldBackupMap.erase(fieldID);
	// 		}
	//
	// 		s_ScriptFieldBackup.erase(entityID);
	// 	}
	// }

	// void ScriptEngine::SetCurrentFieldStates(const UUID& sceneID)
	// {
	// 	if (s_Data->ScriptInstanceMap.find(sceneID) != s_Data->ScriptInstanceMap.end())
	// 	{
	// 		std::unordered_map<UUID, ScriptableInstance> entityInstanceMap = s_Data->ScriptInstanceMap.at(sceneID);
	// 		for (auto& [id, scriptableInstance] : entityInstanceMap)
	// 		{
	// 			std::unordered_map<uint32_t, Utils::Variant> fieldBackupMap;
	// 			const GCHandle handle = GetScriptInstanceGCHandle(sceneID, id);
	// 			Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);
	// 			auto& scriptComponent = entity.GetComponent<ScriptComponent>();
	// 			
	// 			for (const auto& fieldID : scriptComponent.PublicFieldIDs)
	// 			{
	// 				ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);
	// 				Utils::Variant fieldBackup = field->GetData<Utils::Variant>(handle);
	//
	// 				fieldBackupMap.emplace(fieldID, fieldBackup);
	// 			}
	//
	// 			s_ScriptFieldBackup.emplace(id, std::move(fieldBackupMap));
	// 		}
	// 	}
	// }

	static void OnLogMono(const char* logDomain, const char* logLevel, const char* message, mono_bool fatal, void* userData) 
	{
		if (logLevel != nullptr) 
		{
			if (strcmp(logLevel, "info") == 0)
				TR_INFO("Domain: {0}; Message: {1}", logDomain, message);
			else if(strcmp(logLevel, "debug") == 0)
				TR_TRACE("Domain: {0}; Message: {1}", logDomain, message);
			else
				TR_TRACE("Domain: {0}; Message: {1}; Log Level: {2}", logDomain, message, logLevel);
		}
	}

	static void UnhandledExceptionHook(MonoObject* exc, void *user_data)
	{
		ScriptObject object(exc);
	}
}
