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
#include "ScriptUtils.h"

#include "Core/Log.h"
#include "Core/FileUtils.h"
#include "Project/Project.h"

#include "Scene/Components.h"
#include "Scene/SceneManager.h"

#include "Utils/Variant.h"

#include "Utils/Debug/OptickProfiler.h"

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
		ScriptMethodThunks<float> UpdateMethod;
		
		ScriptMethodThunks<MonoArray*> PhysicsBeginContact;
		ScriptMethodThunks<MonoArray*> PhysicsEndContact;

		ScriptMethodThunks<> PhysicsUpdateMethod;

        // TODO: make static?
		void GetMethods() 
		{
			Constructor.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":.ctor(byte[])"));

			InitMethod.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":Init()"));
			UpdateMethod.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":Update(single)"));

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

		std::array<Shared<ScriptAssembly>, TR_ASSEMBLIES> Assemblies;
		
		std::filesystem::path MonoPath = "mono";

		std::filesystem::path LibPath = MonoPath / "lib";
		std::filesystem::path EtcPath = MonoPath / "etc";

		std::filesystem::path MonoConfigPath = EtcPath / "config";
	
		ScriptInstanceMap ScriptInstanceMap;
        std::filesystem::path ScriptCoreAssemblyPath;
		std::function<void(std::string, spdlog::level::level_enum)> LogCallback;
	};

	static void OnLogMono(const char* logDomain, const char* logLevel, const char* message, mono_bool fatal, void* userData);
	
	static ScriptEngineData* s_Data;

	static void Log(const std::string& message, spdlog::level::level_enum logLevel) 
	{
		if (s_Data->LogCallback)
			s_Data->LogCallback(message, logLevel);
	}

	static ScriptableInstance s_EmptyInstance;

	static ScriptableInstance& GetInstance(const UUID& sceneUUID, const UUID& entityUUID) 
	{
		TR_PROFILE_FUNCTION();
		if (s_Data->ScriptInstanceMap.find(sceneUUID) != s_Data->ScriptInstanceMap.end()) 
		{
			if (s_Data->ScriptInstanceMap[sceneUUID].find(entityUUID) != s_Data->ScriptInstanceMap[sceneUUID].end())
				return s_Data->ScriptInstanceMap[sceneUUID][entityUUID];
		}

		return s_EmptyInstance;
	}


	void ScriptEngine::Initialize(const std::filesystem::path& scriptCoreAssemblyPath)
	{
		s_Data = new ScriptEngineData;
        s_Data->ScriptCoreAssemblyPath = scriptCoreAssemblyPath;
		
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
		
		for (size_t i = 0; i < TR_ASSEMBLIES; i++)
			s_Data->Assemblies[i] = CreateShared<ScriptAssembly>();
		
		CreateAppDomain();
		LoadCoreAssembly();
		
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
		std::unordered_map<UUID, std::unordered_map<UUID, std::unordered_map<uint32_t, std::vector<Utils::Variant>>>> arrayScriptFieldsStates;

		for (const auto& [sceneID, scriptInstances] : s_Data->ScriptInstanceMap)
		{
			auto scene = SceneManager::GetScene(sceneID);
			for (const auto& [entityID, instance] : scriptInstances)
			{
				const GCHandle& handle = instance.ObjectHandle;
				Entity entity = scene->FindEntityWithUUID(entityID);

				auto& scriptComponent = entity.GetComponent<ScriptComponent>();

				scriptFieldsStates[sceneID][entityID] = { };
				for (const auto& fieldID : scriptComponent.PublicFieldIDs)
				{
					ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);
					if (field->GetType().IsArray())
					{
						ScriptArray array = field->GetArray(handle);
						arrayScriptFieldsStates[sceneID][entityID][fieldID].reserve(array.Length());

						for (size_t i = 0; i < array.Length(); i++)
							arrayScriptFieldsStates[sceneID][entityID][fieldID].push_back(array[static_cast<uint32_t>(i)]);

						continue;
					}

					scriptFieldsStates[sceneID][entityID][fieldID] = field->GetData<Utils::Variant>(handle);
				}
			}
		}
		
		s_Data->ScriptInstanceMap.clear();
		ScriptBindings::Unbind();

		UnloadDomain();
		CreateAppDomain();

		bool coreAssemblyLoaded = LoadCoreAssembly();
		TR_ASSERT(coreAssemblyLoaded, "Couldn't load the core assembly");
		ScriptBindings::Bind();

		bool appAssemblyLoaded = LoadAppAssembly();
		
		// TODO: if the app assembly couldn't be loaded then return from method
		// this also means that the scriptFieldStates should be saved more globally than the function scope
		// because we still want to set them back in case the assembly was loaded successfuly in the future

		for (const auto& [sceneID, scriptFieldsValues] : scriptFieldsStates)
		{
			auto scene = SceneManager::GetScene(sceneID);
			for (const auto& [entityID, fieldsState] : scriptFieldsValues)
			{
				Entity entity = scene->FindEntityWithUUID(entityID);
				InitializeScriptable(entity);
				
				for (const auto& [fieldID, fieldData] : fieldsState)
				{
					GCHandle handle = GetScriptInstanceGCHandle(sceneID, entityID);

					if(!handle.IsValid())
						break;

					ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);

					if(!field)
						continue;
					
					if (field->GetType().IsArray())
						continue;

					field->SetData<Utils::Variant>(fieldData, handle);
				}
			}
		}
		
		for (const auto& [sceneID, scriptFieldsValues] : arrayScriptFieldsStates)
		{
			auto scene = SceneManager::GetScene(sceneID);
			for (const auto& [entityID, fieldsState] : scriptFieldsValues)
			{
				Entity entity = scene->FindEntityWithUUID(entityID);
				InitializeScriptable(entity);
				
				for (const auto& [fieldID, arrayFieldData] : fieldsState)
				{
					GCHandle handle = GetScriptInstanceGCHandle(sceneID, entityID);

					if(!handle.IsValid())
						break;

					ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);

					if(!field)
						continue;
					
					if (!field->GetType().IsArray())
						continue;

					ScriptArray array = field->GetArray(handle);
					if (array.Length() != arrayFieldData.size())
						array.Resize(arrayFieldData.size());

					for (size_t i = 0; i < arrayFieldData.size(); i++)
						array.Set<Utils::Variant>(static_cast<uint32_t>(i), arrayFieldData[i]);

					field->SetArray(array, handle);
				}
			}
		}

		TR_INFO("Reloaded assemblies!");
		Log("Reloaded assemblies!", spdlog::level::info);
	}

	bool ScriptEngine::LoadCoreAssembly()
	{
		auto& coreAssembly = s_Data->Assemblies.at(TR_CORE_ASSEMBLY_INDEX);
		coreAssembly = ScriptAssembly::LoadAssembly(s_Data->ScriptCoreAssemblyPath);
		TR_ASSERT(coreAssembly, "Couldn't load the TerranScriptCore assembly");

		ScriptCache::CacheCoreClasses();

		return true;
	}

	bool ScriptEngine::LoadAppAssembly()
	{
		auto& appAssembly = s_Data->Assemblies.at(TR_APP_ASSEMBLY_INDEX);
		appAssembly = ScriptAssembly::LoadAssembly(Project::GetAppAssemblyPath());

		if (!appAssembly) 
		{
			Log("Couldn't load the ScriptAssembly assembly", spdlog::level::err);
			return false;
		}

        Shared<AssemblyInfo> assemblyInfo = appAssembly->GenerateAssemblyInfo();
        ScriptCache::GenerateCacheForAssembly(assemblyInfo);

		return true;
	}

	void ScriptEngine::SetLogCallback(LogFN logCallback) { s_Data->LogCallback = logCallback; }

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
			
			mono_domain_unload(s_Data->AppDomain);
		}

		ScriptCache::ClearCache();
	}

	ScriptClass ScriptEngine::GetClassFromName(const std::string& moduleName, int assemblyIndex) { return s_Data->Assemblies[assemblyIndex]->GetClassFromName(moduleName); }
	ScriptClass ScriptEngine::GetClassFromTypeToken(uint32_t typeToken, int assemblyIndex) { return s_Data->Assemblies[assemblyIndex]->GetClassFromTypeToken(typeToken); }
	ScriptMethod ScriptEngine::GetMethodFromDesc(const std::string& methodDesc, int assemblyIndex) { return s_Data->Assemblies[assemblyIndex]->GetMethodFromDesc(methodDesc); }
	bool ScriptEngine::ClassExists(const std::string& moduleName) { return ScriptCache::GetCachedClassFromName(moduleName); }
	Shared<ScriptAssembly>& ScriptEngine::GetAssembly(int assemblyIndex) { return s_Data->Assemblies.at(assemblyIndex); }

	GCHandle ScriptEngine::InitializeScriptable(Entity entity)
	{
		TR_PROFILE_FUNCTION();
		auto& scriptComponent = entity.GetComponent<ScriptComponent>();

		if (scriptComponent.ModuleName.empty()) return { };

		if (!ScriptEngine::ClassExists(scriptComponent.ModuleName)) 
		{
			scriptComponent.ClassExists = false;
			TR_ERROR("Class {0} doesn't exist", scriptComponent.ModuleName);
			return { };
		}

		scriptComponent.ClassExists = true;

		if (s_Data->ScriptInstanceMap.find(entity.GetID()) != s_Data->ScriptInstanceMap.end()) 
			return { };

		ScriptClass* klass = ScriptCache::GetCachedClassFromName(scriptComponent.ModuleName);
			
		if (!klass) return { };

		if (klass->IsInstanceOf(TR_API_CACHED_CLASS(Scriptable)))
		{
			std::string errorMessage = fmt::format("Class {0} doesn't extend Scriptable", scriptComponent.ModuleName);
			Log(errorMessage, spdlog::level::warn);
			return { };
		}

		ScriptableInstance instance;
		const ScriptObject object = ScriptObject::CreateInstace(*klass);
		instance.ObjectHandle = GCManager::CreateStrongHadle(object);
		instance.GetMethods();

		ScriptArray uuidArray = ScriptMarshal::UUIDToMonoArray(entity.GetID());
			
		MonoException* exception = nullptr;
		instance.Constructor.Invoke(object.GetMonoObject(), uuidArray.GetMonoArray(), &exception);

		if (exception) 
		{
			Log(ScriptUtils::GetExceptionMessage(exception), spdlog::level::err);
			return { };
		}
			
		s_Data->ScriptInstanceMap[entity.GetSceneID()][entity.GetID()] = instance;

		scriptComponent.PublicFieldIDs.clear();
		for (ScriptField& field : klass->GetFields())
		{
			if(field.GetVisibility() == ScriptFieldVisibility::Public)
				scriptComponent.PublicFieldIDs.emplace_back(field.GetID());
		}

		return instance.ObjectHandle;
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
		TR_PROFILE_FUNCTION();
		ScriptableInstance instance = GetInstance(entity.GetSceneID(), entity.GetID());

		if (instance.InitMethod)
		{
			MonoException* exception = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.InitMethod.Invoke(monoObject, &exception);

			if(exception)
				Log(ScriptUtils::GetExceptionMessage(exception), spdlog::level::err);
		}
	}

	void ScriptEngine::OnUpdate(Entity entity, float deltaTime) 
	{
		TR_PROFILE_FUNCTION();
		ScriptableInstance instance = GetInstance(entity.GetSceneID(), entity.GetID());

		if (instance.UpdateMethod) 
		{
			MonoException* exception = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.UpdateMethod.Invoke(monoObject, deltaTime, &exception);

			if(exception)
				Log(ScriptUtils::GetExceptionMessage(exception), spdlog::level::err);
		}
	}

	void ScriptEngine::OnPhysicsBeginContact(Entity collider, Entity collidee)
	{
		TR_PROFILE_FUNCTION();
		ScriptableInstance instance = GetInstance(collider.GetSceneID(), collider.GetID());

		if (instance.PhysicsBeginContact) 
		{
			ScriptArray uuidArr = ScriptMarshal::UUIDToMonoArray(collidee.GetID());
			MonoException* exception = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.PhysicsBeginContact.Invoke(monoObject, uuidArr.GetMonoArray(), &exception);

			if(exception)
				Log(ScriptUtils::GetExceptionMessage(exception), spdlog::level::err);
		}
	}

	void ScriptEngine::OnPhysicsEndContact(Entity collider, Entity collidee)
	{
		TR_PROFILE_FUNCTION();
		ScriptableInstance instance = GetInstance(collider.GetSceneID(), collider.GetID());

		if (instance.PhysicsEndContact)
		{
			ScriptArray uuidArr = ScriptMarshal::UUIDToMonoArray(collidee.GetID());
			MonoException* exception = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.PhysicsEndContact.Invoke(monoObject, uuidArr.GetMonoArray(), &exception);

			if(exception)
				Log(ScriptUtils::GetExceptionMessage(exception), spdlog::level::err);
		}
	}

	void ScriptEngine::OnPhysicsUpdate(Entity entity)
	{
		TR_PROFILE_FUNCTION();
		ScriptableInstance instance = GetInstance(entity.GetSceneID(), entity.GetID());

		if (instance.PhysicsUpdateMethod) 
		{
			MonoException* exception = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.PhysicsUpdateMethod.Invoke(monoObject, &exception);

			if(exception)
				Log(ScriptUtils::GetExceptionMessage(exception), spdlog::level::err);
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
