#include "trpch.h"
#include "ScriptEngine.h"

#include "ScriptBindings.h"
//#include "ScriptMarshal.h"
//#include "GCManager.h"
//#include "ScriptCache.h"
//#include "ManagedMethodThunks.h"
//#include "ScriptAssembly.h"
//#include "ManagedObject.h"
//#include "ScriptArray.h"
//#include "ScriptUtils.h"
//#include "ManagedMetadata.h"

#include "Core/Log.h"
#include "Core/FileUtils.h"
#include "Project/Project.h"

#include "Scene/Components.h"
#include "Scene/SceneManager.h"

#include "Utils/Variant.h"

#include "Utils/Debug/OptickProfiler.h"

#include <unordered_map>

#include <Coral/HostInstance.hpp>
#include <Coral/GC.hpp>
#include <Coral/Array.hpp>
#include <Coral/TypeCache.hpp>

#include <oaidl.h>

namespace TerranEngine
{
	//struct ScriptableInstance 
	//{
	//	GCHandle ObjectHandle;
	//	ManagedMethodThunks<MonoArray*> Constructor;
	//	ManagedMethodThunks<> InitMethod;
	//	ManagedMethodThunks<float> UpdateMethod;
	//	
	//	ManagedMethodThunks<MonoArray*> PhysicsBeginContact;
	//	ManagedMethodThunks<MonoArray*> PhysicsEndContact;

	//	ManagedMethodThunks<> PhysicsUpdateMethod;

 //       // TODO: make static?
	//	void GetMethods() 
	//	{
	//		Constructor.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":.ctor(byte[])"));

	//		InitMethod.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":Init()"));
	//		UpdateMethod.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":Update(single)"));

	//		PhysicsBeginContact.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":OnCollisionBegin(Entity)"));
	//		PhysicsEndContact.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":OnCollisionEnd(Entity)"));

	//		PhysicsUpdateMethod.SetFromMethod(ScriptCache::GetCachedMethod("Terran.Scriptable", ":PhysicsUpdate()"));
	//	}
	//};
	
	using ScriptInstanceMap = std::unordered_map<UUID, std::unordered_map<UUID, Shared<ScriptInstance>>>;
	
	struct ScriptEngineData 
	{
		Coral::HostInstance HostInstance;
		Coral::AssemblyLoadContext LoadContext;
		std::array<Coral::ManagedAssembly, TR_ASSEMBLIES> Assemblies;
		
		std::string CoralDirectory = "Resources/Scripts";
		Coral::Type ScriptableBaseClass;

		ScriptInstanceMap ScriptInstanceMap;
        std::filesystem::path ScriptCoreAssemblyPath;
		std::function<void(std::string, spdlog::level::level_enum)> LogCallback;
		std::unordered_map<Coral::TypeId, ScriptType> TypeConverters;
	};

	//static void OnLogMono(const char* logDomain, const char* logLevel, const char* message, mono_bool fatal, void* userData);
	
	static ScriptEngineData* s_Data;

	static void Log(const std::string& message, spdlog::level::level_enum logLevel) 
	{
		if (s_Data->LogCallback)
			s_Data->LogCallback(message, logLevel);
	}

	//static ScriptableInstance s_EmptyInstance;

	/*static ScriptableInstance& GetInstance(const UUID& sceneUUID, const UUID& entityUUID) 
	{
		TR_PROFILE_FUNCTION();
		if (s_Data->ScriptInstanceMap.find(sceneUUID) != s_Data->ScriptInstanceMap.end()) 
		{
			if (s_Data->ScriptInstanceMap[sceneUUID].find(entityUUID) != s_Data->ScriptInstanceMap[sceneUUID].end())
				return s_Data->ScriptInstanceMap[sceneUUID][entityUUID];
		}

		return s_EmptyInstance;
	}*/

	static void OnException(std::string_view message) 
	{
		TR_ERROR(message);
	}

	static void OnMessage(std::string_view message, Coral::MessageLevel messageLevel) 
	{
		switch (messageLevel)
		{
		case Coral::MessageLevel::Info:		TR_INFO(message); break;
		case Coral::MessageLevel::Warning:	TR_WARN(message); break;
		case Coral::MessageLevel::Error:	TR_ERROR(message); break;
		}
	}

	void ScriptEngine::Initialize(const std::filesystem::path& scriptCoreAssemblyPath) 
	{
		s_Data = new ScriptEngineData;
		s_Data->ScriptCoreAssemblyPath = scriptCoreAssemblyPath;

		Coral::HostSettings settings = 
		{
			.CoralDirectory = s_Data->CoralDirectory,
			.MessageCallback = OnMessage,
			.ExceptionCallback = OnException
		};

		s_Data->HostInstance.Initialize(settings);
		s_Data->LoadContext = s_Data->HostInstance.CreateAssemblyLoadContext("ScriptAppContext");

		LoadCoreAssembly();
	}

	bool ScriptEngine::LoadCoreAssembly() 
	{
		auto& coreAssembly = s_Data->Assemblies.at(TR_CORE_ASSEMBLY_INDEX);
		coreAssembly = s_Data->LoadContext.LoadAssembly(s_Data->ScriptCoreAssemblyPath.string());
		TR_ASSERT(coreAssembly.GetLoadStatus() == Coral::AssemblyLoadStatus::Success, "Couldn't load the TerranScriptCore assembly");

		s_Data->ScriptableBaseClass = coreAssembly.GetType("Terran.Scriptable");
		if (!s_Data->ScriptableBaseClass)
			TR_ERROR("The scriptable base class wasn't found!");

		InitializeTypeConverters();
		return true;
	}

	void ScriptEngine::Shutdown()
	{
		Coral::GC::Collect();
		s_Data->HostInstance.UnloadAssemblyLoadContext(s_Data->LoadContext);
	}

	void ScriptEngine::ReloadAppAssembly() {}

	bool ScriptEngine::ClassExists(const std::string& moduleName) { return false;  }

	//Shared<ScriptAssembly> ScriptEngine::GetAssembly(int assemblyIndex) { return nullptr; }

	ScriptType ScriptEngine::GetScriptType(Coral::Type& type)
	{
		Coral::ManagedType managedType = type.GetManagedType();
		if (managedType != Coral::ManagedType::Unknown || managedType != Coral::ManagedType::Pointer)
			return (ScriptType)managedType;

		if (s_Data->TypeConverters.find(type.GetTypeId()) != s_Data->TypeConverters.end())
			return s_Data->TypeConverters.at(type.GetTypeId());

		return ScriptType::None;
	}

#define ADD_SYSTEM_TYPE(TypeName, Type)\
	typeConverters.emplace(typeCache.GetTypeByName("System."#TypeName)->GetTypeId(), ScriptType::Type);

#define ADD_TERRAN_TYPE(TypeName)\
	typeConverters.emplace(typeCache.GetTypeByName("Terran."#TypeName)->GetTypeId(), ScriptType::TypeName);

	void ScriptEngine::InitializeTypeConverters()
	{
		auto& typeCache = Coral::TypeCache::Get();
		auto& typeConverters = s_Data->TypeConverters;
		/*ADD_SYSTEM_TYPE(Byte, UInt8);
		ADD_SYSTEM_TYPE(UInt16, UInt16);
		ADD_SYSTEM_TYPE(UInt32, UInt32);
		ADD_SYSTEM_TYPE(UInt64, UInt64);
		
		ADD_SYSTEM_TYPE(SByte, Int8);
		ADD_SYSTEM_TYPE(Int16, Int16);
		ADD_SYSTEM_TYPE(Int32, Int32);
		ADD_SYSTEM_TYPE(Int64, Int64);

		ADD_SYSTEM_TYPE(Single, Float);
		ADD_SYSTEM_TYPE(Double, Double);

		ADD_SYSTEM_TYPE(Boolean, Bool);
		ADD_SYSTEM_TYPE(Char, Char);*/

		ADD_TERRAN_TYPE(Vector2);
		ADD_TERRAN_TYPE(Vector3);
		ADD_TERRAN_TYPE(Color);
		ADD_TERRAN_TYPE(Entity);
	}

	Shared<ScriptInstance> ScriptEngine::InitializeScriptable(Entity entity) 
	{
		auto& scriptComponent = entity.GetComponent<ScriptComponent>();

		if (scriptComponent.ModuleName.empty()) return nullptr;

		Coral::ManagedAssembly& appAssembly = s_Data->Assemblies.at(TR_APP_ASSEMBLY_INDEX);
		Coral::Type& type = appAssembly.GetType(scriptComponent.ModuleName);

		if (!type)
		{
			scriptComponent.ClassExists = false;
			TR_ERROR("Class {0} doesn't exist", scriptComponent.ModuleName);
			return nullptr;
		}

		scriptComponent.ClassExists = true;

		if (s_Data->ScriptInstanceMap.find(entity.GetSceneID()) != s_Data->ScriptInstanceMap.end())
		{
			auto obj = s_Data->ScriptInstanceMap.at(entity.GetSceneID()).find(entity.GetID());
			if (obj != s_Data->ScriptInstanceMap.at(entity.GetSceneID()).end())
				return ((*obj).second);
		}

		if (!type.IsSubclassOf(s_Data->ScriptableBaseClass))
		{
			TR_ERROR("Class {0} doesn not extend Scriptable", scriptComponent.ModuleName);
			return nullptr;
		}

		//Coral::ManagedObject object = type.CreateInstance(entity.GetID());
		Shared<ScriptInstance> instance =
			s_Data->ScriptInstanceMap[entity.GetSceneID()][entity.GetID()] =
			CreateShared<ScriptInstance>(type, entity);

		scriptComponent.PublicFieldIDs.clear();
		for (Coral::FieldInfo& fieldInfo : type.GetFields())
		{
			if (fieldInfo.GetAccessibility() == Coral::TypeAccessibility::Public)
			{
				scriptComponent.PublicFieldIDs.emplace_back(fieldInfo.GetHandle());
				Coral::ScopedString fieldName = fieldInfo.GetName();
				ScriptField field = { GetScriptType(fieldInfo.GetType()), fieldName };
				instance->m_Fields.emplace(fieldInfo.GetHandle(), field);
			}
		}

		Shared<ScriptInstance> i2 = s_Data->ScriptInstanceMap.at(entity.GetSceneID()).at(entity.GetID());

		return s_Data->ScriptInstanceMap.at(entity.GetSceneID()).at(entity.GetID());
	}

	void ScriptEngine::UninitalizeScriptable(Entity entity) {}

	void ScriptEngine::OnStart(Entity entity) {}
	void ScriptEngine::OnUpdate(Entity entity, float deltaTime) {}

	void ScriptEngine::OnPhysicsBeginContact(Entity collider, Entity collidee) {}
	void ScriptEngine::OnPhysicsEndContact(Entity collider, Entity collidee) {}

	void ScriptEngine::OnPhysicsUpdate(Entity entity) {}

	//ManagedObject ScriptEngine::GetScriptInstanceScriptObject(const UUID& sceneUUID, const UUID& entityUUID) { return {}; }
	//GCHandle ScriptEngine::GetScriptInstanceGCHandle(const UUID& sceneUUID, const UUID& entityUUID) { return {}; }

	bool ScriptEngine::LoadAppAssembly() 
	{
		auto& appAssembly = s_Data->Assemblies.at(TR_APP_ASSEMBLY_INDEX);
		appAssembly = s_Data->LoadContext.LoadAssembly(Project::GetAppAssemblyPath().string());

		Coral::AssemblyLoadStatus status = appAssembly.GetLoadStatus();

		if (status != Coral::AssemblyLoadStatus::Success)
		{
			TR_ERROR("Couldn't load the ScriptAssembly assembly");
			return false;
		}

		return true;
	}

#if 0
	void ScriptEngine::Initialize(const std::filesystem::path& scriptCoreAssemblyPath)
	{
		s_Data = new ScriptEngineData;
        s_Data->ScriptCoreAssemblyPath = scriptCoreAssemblyPath;
		
		mono_set_dirs(s_Data->LibPath.string().c_str(), s_Data->EtcPath.string().c_str());

#ifdef TR_DEBUG
		// TODO: attach deugger
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

		// TODO:
		//delete s_Data;

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
				Entity entity = scene->FindEntityWithUUID(entityID);

				auto& scriptComponent = entity.GetComponent<ScriptComponent>();

				scriptFieldsStates[sceneID][entityID] = { };
				for (const auto& fieldID : scriptComponent.PublicFieldIDs)
				{
					ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);
					if (field->GetType().IsArray())
					{
						ScriptArray array = field->GetArray(instance.ObjectHandle);
						arrayScriptFieldsStates[sceneID][entityID][fieldID].reserve(array.Length());

						for (size_t i = 0; i < array.Length(); i++)
							arrayScriptFieldsStates[sceneID][entityID][fieldID].push_back(array[static_cast<uint32_t>(i)]);

						continue;
					}

					scriptFieldsStates[sceneID][entityID][fieldID] = field->GetData<Utils::Variant>(instance.ObjectHandle);
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
		if (!appAssemblyLoaded)
			return;

		// TODO: if the app assembly couldn't be loaded then return from method
		// this also means that the scriptFieldStates should be saved more globally than the function scope
		// because we still want to set them back in case the assembly was loaded successfuly in the future

		for (const auto& [sceneId, scene] : SceneManager::GetActiveScenes())
		{
			auto view = scene->GetEntitiesWith<ScriptComponent>();
			for (const auto e : view)
			{
				Entity entity(e, scene.get());
				ScriptComponent& scriptComponent = entity.GetComponent<ScriptComponent>();
				const UUID& entityId = entity.GetID();
				GCHandle handle = InitializeScriptable(entity);
				if (!handle) continue;

				ManagedClass* klass = ScriptCache::GetCachedClassFromName(scriptComponent.ModuleName);

				for (ScriptField& field : klass->GetFields())
				{
					if (field.GetType().IsArray())
					{
						ScriptArray array = field.GetArray(handle);

						try
						{
							const std::vector<Utils::Variant>& cachedFieldArrayData = arrayScriptFieldsStates.at(sceneId).at(entityId).at(field.GetID());
							if (array.Length() != cachedFieldArrayData.size())
								array.Resize(cachedFieldArrayData.size());

							for (size_t i = 0; i < cachedFieldArrayData.size(); i++)
								array.Set<Utils::Variant>(static_cast<uint32_t>(i), cachedFieldArrayData[i]);

							field.SetArray(array, handle);
							continue;
						}
						catch (std::out_of_range e)
						{
							// the scene, entity or field doesn't exist in the cached array fields' values
							continue;
						}
					}

					try
					{
						const Utils::Variant& fieldData = scriptFieldsStates.at(sceneId).at(entityId).at(field.GetID());
						field.SetData<Utils::Variant>(fieldData, handle);
					}
					catch (std::out_of_range e)
					{
						// the scene, entity or field doesn't exist in the cached fields' values
						continue;
					}
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

        ScriptCache::GenerateCacheForAssembly(appAssembly);

		return true;
	}

	void ScriptEngine::SetLogCallback(LogFN logCallback) { s_Data->LogCallback = logCallback; }

	void ScriptEngine::CreateAppDomain() 
	{
		s_Data->AppDomain = mono_domain_create_appdomain((char*)"ScriptAssemblyDomain", NULL);
		
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

	ManagedClass ScriptEngine::GetClassFromName(const std::string& moduleName, int assemblyIndex) { return s_Data->Assemblies[assemblyIndex]->GetClassFromName(moduleName); }
	ManagedClass ScriptEngine::GetClassFromTypeToken(uint32_t typeToken, int assemblyIndex) { return s_Data->Assemblies[assemblyIndex]->GetClassFromTypeToken(typeToken); }
	ManagedMethod ScriptEngine::GetMethodFromDesc(const std::string& methodDesc, int assemblyIndex) { return s_Data->Assemblies[assemblyIndex]->GetMethodFromDesc(methodDesc); }
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

		ManagedClass* klass = ScriptCache::GetCachedClassFromName(scriptComponent.ModuleName);
			
		if (!klass) return { };

		if (klass->IsInstanceOf(TR_API_CACHED_CLASS(Scriptable)))
		{
			std::string errorMessage = fmt::format("Class {0} doesn't extend Scriptable", scriptComponent.ModuleName);
			Log(errorMessage, spdlog::level::warn);
			return { };
		}

		ScriptableInstance instance;
		const ManagedObject object = klass->CreateInstance();
		instance.ObjectHandle = GCManager::CreateStrongHandle(object);
		instance.GetMethods();

		MonoArray* uuidArray = ScriptMarshal::UUIDToMonoArray(entity.GetID());
		MonoException* exception = nullptr;
		instance.Constructor.Invoke(object.GetMonoObject(), uuidArray, &exception);

		if (exception) 
		{
			Log(ScriptUtils::GetExceptionMessage(exception), spdlog::level::err);
			return { };
		}
			
		s_Data->ScriptInstanceMap[entity.GetSceneID()][entity.GetID()] = instance;

		scriptComponent.PublicFieldIDs.clear();
		for (ScriptField& field : klass->GetFields())
		{
			if (field.GetVisibility() == ScriptFieldVisibility::Public) 
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
			MonoArray* uuidData = ScriptMarshal::UUIDToMonoArray(collidee.GetID());
			MonoException* exception = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.PhysicsBeginContact.Invoke(monoObject, uuidData, &exception);

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
			MonoArray* uuidData = ScriptMarshal::UUIDToMonoArray(collidee.GetID());
			MonoException* exception = nullptr;
			MonoObject* monoObject = GCManager::GetManagedObject(instance.ObjectHandle);
			instance.PhysicsEndContact.Invoke(monoObject, uuidData, &exception);

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

	ManagedObject ScriptEngine::GetScriptInstanceScriptObject(const UUID& sceneUUID, const UUID& entityUUID)
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
#endif
}
