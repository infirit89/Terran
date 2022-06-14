#include "trpch.h"
#include "ScriptEngine.h"

#include "ScriptBindings.h"
#include "ScriptMarshal.h"
#include "GCManager.h"
#include "ScriptCache.h"
#include "ScriptMethodThunks.h"
#include "ScriptAssembly.h"

#include "Core/Log.h"
#include "Core/FileUtils.h"

#include "Scene/Components.h"
#include "Scene/SceneManager.h"

#include "Utils/Utils.h"

#include <glm/glm.hpp>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/mono-debug.h>
#include <mono/utils/mono-logger.h>
#include <mono/metadata/mono-config.h>

#include <unordered_map>

namespace TerranEngine
{
	struct ScriptEngineData 
	{
		MonoDomain* CoreDomain;
		MonoDomain* NewDomain;

		Shared<ScriptAssembly> Assembly;

		std::filesystem::path AssemblyPath;
		std::filesystem::path MonoPath = "mono";

		std::filesystem::path LibPath = MonoPath / "lib";
		std::filesystem::path EtcPath = MonoPath / "etc";

		std::filesystem::path MonoConfigPath = EtcPath / "config";
	};

	static void OnLogMono(const char* logDomain, const char* logLevel, const char* message, mono_bool fatal, void* userData);

	static ScriptEngineData s_ScriptEngineData;

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

	static ScriptableInstance s_EmptyInstance;

	static std::unordered_map<UUID, std::unordered_map<UUID, ScriptableInstance>> s_ScriptableInstanceMap;

	static std::unordered_map<UUID, std::unordered_map<uint32_t, Utils::Variant>> s_ScriptFieldBackup;

	static ScriptableInstance& GetInstance(const UUID& sceneUUID, const UUID& entityUUID) 
	{
		if (s_ScriptableInstanceMap.find(sceneUUID) != s_ScriptableInstanceMap.end()) 
		{
			if (s_ScriptableInstanceMap[sceneUUID].find(entityUUID) != s_ScriptableInstanceMap[sceneUUID].end())
				return s_ScriptableInstanceMap[sceneUUID][entityUUID];
		}

		return s_EmptyInstance;
	}


	void ScriptEngine::Initialize(const std::filesystem::path& assemblyPath)
	{
		s_ScriptEngineData.AssemblyPath = assemblyPath;
		
		mono_set_dirs(s_ScriptEngineData.LibPath.string().c_str(), s_ScriptEngineData.EtcPath.string().c_str());

#ifdef TR_DEBUG
		mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		mono_trace_set_level_string("debug");
		mono_trace_set_log_handler(OnLogMono, nullptr);
#endif

		mono_config_parse(s_ScriptEngineData.MonoConfigPath.string().c_str());

		s_ScriptEngineData.CoreDomain = mono_jit_init("CoreDomain");
		
		if (!s_ScriptEngineData.CoreDomain)
		{
			TR_ERROR("Couldn't initialize the Mono Domain!");
			return;
		}

		NewDomain();
		
		Shared<AssemblyInfo> assemblyInfo = s_ScriptEngineData.Assembly->GenerateAssemblyInfo();

		ScriptCache::CacheClassesFromAssemblyInfo(assemblyInfo);
		ScriptCache::CacheMethodsFromAssemblyInfo(assemblyInfo);
		ScriptCache::CacheFieldsFromAssemblyInfo(assemblyInfo);
	}

	void ScriptEngine::Shutdown()
	{
		mono_jit_cleanup(s_ScriptEngineData.CoreDomain);
		TR_INFO("Deinitialized the scripting core");
	}

	void ScriptEngine::NewDomain() 
	{
		s_ScriptEngineData.NewDomain = mono_domain_create_appdomain("ScriptAssemblyDomain", NULL);
		
		if (!mono_domain_set(s_ScriptEngineData.NewDomain, false))
		{
			TR_ERROR("Couldn't set the new domain");
			return;
		}

		s_ScriptEngineData.Assembly = ScriptAssembly::LoadScriptAssembly(s_ScriptEngineData.AssemblyPath);
		TR_INFO("Successfuly loaded: {0}", s_ScriptEngineData.AssemblyPath);

		ScriptCache::CacheCoreClasses();
		ScriptBindings::Bind();
	}

	std::filesystem::path ScriptEngine::GetAssemblyPath()
	{
		return s_ScriptEngineData.AssemblyPath;
	}

	void ScriptEngine::UnloadDomain() 
	{
		if (s_ScriptEngineData.NewDomain != s_ScriptEngineData.CoreDomain)
		{
			SetCurrentFieldStates(SceneManager::GetCurrentScene()->GetID());

			mono_domain_set(s_ScriptEngineData.CoreDomain, false);

			if (!mono_domain_finalize(s_ScriptEngineData.NewDomain, 2000))
			{
				TR_ERROR("Finalizing the domain timed out");
				return;
			}
			mono_gc_collect(mono_gc_max_generation());

			const auto scriptView = SceneManager::GetCurrentScene()->GetEntitiesWith<ScriptComponent>();

			for (auto e : scriptView)
			{
				Entity entity(e, SceneManager::GetCurrentScene().get());
				UninitalizeScriptable(entity);
			}

			mono_domain_unload(s_ScriptEngineData.NewDomain);
		}

		ScriptCache::ClearCache();
	}

	ScriptClass ScriptEngine::GetClassFromName(const std::string& moduleName)
	{
		return s_ScriptEngineData.Assembly->GetClassFromName(moduleName);
	}

	ScriptClass ScriptEngine::GetClassFromTypeToken(uint32_t typeToken)
	{
		return s_ScriptEngineData.Assembly->GetClassFromTypeToken(typeToken);
	}

	ScriptMethod ScriptEngine::GetMethodFromDesc(const std::string& methodDesc)
	{
		return s_ScriptEngineData.Assembly->GetMethodFromDesc(methodDesc);
	}

	bool ScriptEngine::ClassExists(const std::string& moduleName)
	{
		return ScriptCache::GetCachedClassFromName(moduleName);
	}

	void ScriptEngine::InitializeScriptable(Entity entity)
	{
		if (s_ScriptableInstanceMap.find(entity.GetID()) == s_ScriptableInstanceMap.end())
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

			MonoArray* uuidArray = ScriptMarshal::UUIDToMonoArray(entity.GetID());

			MonoException* exc = nullptr;
			instance.Constructor.Invoke(object.GetMonoObject(), uuidArray, &exc);

			s_ScriptableInstanceMap[entity.GetSceneID()][entity.GetID()] = instance;

			// TODO: fix the rest of the field code
			TR_TRACE(klass->GetFields().size());
			scriptComponent.PublicFieldIDs.clear();
			
			for (const auto& field : klass->GetFields())
			{
				if(field.GetVisibility() == ScriptFieldVisibility::Public)
					scriptComponent.PublicFieldIDs.emplace_back(field.GetID());
			}


			for (const auto& fieldID : scriptComponent.PublicFieldIDs)
			{
				if(s_ScriptFieldBackup.find(entity.GetID()) == s_ScriptFieldBackup.end())
					break;

				std::unordered_map<uint32_t, Utils::Variant>& fieldBackup = s_ScriptFieldBackup.at(entity.GetID());
				if(fieldBackup.find(fieldID) == fieldBackup.end())
					continue;
				ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID); 
				if(field->GetType() == (ScriptFieldType)fieldBackup.at(fieldID).GetType())
					field->SetData<Utils::Variant>(fieldBackup.at(fieldID), instance.ObjectHandle);
			}
			
		}
	}

	void ScriptEngine::UninitalizeScriptable(Entity entity)
	{
		if (!entity || !entity.HasComponent<TagComponent>())
			return;

		if (s_ScriptableInstanceMap.find(entity.GetSceneID()) != s_ScriptableInstanceMap.end()) 
		{
			if (s_ScriptableInstanceMap[entity.GetSceneID()].find(entity.GetID()) != s_ScriptableInstanceMap[entity.GetSceneID()].end()) 
			{
				GCHandle& handle = s_ScriptableInstanceMap[entity.GetSceneID()][entity.GetID()].ObjectHandle;
				GCManager::FreeHandle(handle);
				s_ScriptableInstanceMap[entity.GetSceneID()].erase(entity.GetID());
			}
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
			MonoArray* uuidArr = ScriptMarshal::UUIDToMonoArray(collidee.GetID());
			MonoException* exc = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.PhysicsBeginContact.Invoke(monoObject, uuidArr, &exc);
		}
	}

	void ScriptEngine::OnPhysicsEndContact(Entity collider, Entity collidee)
	{
		ScriptableInstance instance = GetInstance(collider.GetSceneID(), collider.GetID());

		if (instance.PhysicsEndContact)
		{
			MonoArray* uuidArr = ScriptMarshal::UUIDToMonoArray(collidee.GetID());
			MonoException* exc = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.PhysicsBeginContact.Invoke(monoObject, uuidArr, &exc);
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

	void ScriptEngine::ClearFieldBackupMap()
	{
		auto scriptFieldBackupCpy = s_ScriptFieldBackup;
		for (auto& [entityID, fieldBackupMap] : scriptFieldBackupCpy)
		{
			auto fieldBackupCpy = fieldBackupMap;
			for (auto& [fieldID, fieldData] : fieldBackupCpy)
			{
				fieldData.Clear();
				fieldBackupMap.erase(fieldID);
			}

			s_ScriptFieldBackup.erase(entityID);
		}
	}

	void ScriptEngine::SetCurrentFieldStates(const UUID& sceneID)
	{
		if (s_ScriptableInstanceMap.find(sceneID) != s_ScriptableInstanceMap.end())
		{
			std::unordered_map<UUID, ScriptableInstance> entityInstanceMap = s_ScriptableInstanceMap.at(sceneID);
			for (auto& [id, scriptableInstance] : entityInstanceMap)
			{
				std::unordered_map<uint32_t, Utils::Variant> fieldBackupMap;
				const GCHandle handle = GetScriptInstanceGCHandle(sceneID, id);
				Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);
				auto& scriptComponent = entity.GetComponent<ScriptComponent>();
				
				for (const auto& fieldID : scriptComponent.PublicFieldIDs)
				{
					ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);
					Utils::Variant fieldBackup = field->GetData<Utils::Variant>(handle);

					fieldBackupMap.emplace(fieldID, fieldBackup);
				}

				s_ScriptFieldBackup.emplace(id, std::move(fieldBackupMap));
			}
		}
	}

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
}
