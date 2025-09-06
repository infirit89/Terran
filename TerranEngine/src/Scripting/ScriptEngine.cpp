#include "trpch.h"
#include "ScriptEngine.h"

#include "ScriptBindings.h"
#include "ScriptTypes.h"

#include "Core/Application.h"
#include "Core/Log.h"

#include "Events/ScriptEngineEvent.h"

#include "Project/Project.h"

#include "Scene/Components.h"
#include "Scene/SceneManager.h"

#include "Utils/Variant.h"

#include "Utils/Debug/OptickProfiler.h"

#include <unordered_map>

#include <Coral/HostInstance.hpp>
#include <Coral/GC.hpp>
#include <Coral/TypeCache.hpp>


namespace TerranEngine
{
	using ScriptInstanceMap = std::unordered_map<UUID, std::unordered_map<UUID, Shared<ScriptInstance>>>;
	
	struct ScriptEngineData final
	{
		Coral::HostInstance HostInstance;
		Coral::AssemblyLoadContext LoadContext;
		std::array<Coral::ManagedAssembly, TR_ASSEMBLIES> Assemblies;
		
		std::string CoralDirectory = "Resources/Scripts";
		int32_t EntityIDFieldHandle = 0;

		ScriptInstanceMap ScriptInstanceMap; 
        std::filesystem::path ScriptCoreAssemblyPath;
		std::unordered_map<Coral::TypeId, ScriptFieldType> TypeConverters;
		std::unordered_map<std::string, Coral::Type> ScriptTypeMap;
	};

	static ScriptEngineData* s_Data;

	static void Log(std::string_view message, spdlog::level::level_enum logLevel) 
	{
		ScriptEngineLogEvent logEvent(message, logLevel);
		Application::Get()->DispatchEvent(logEvent);
	}

	static void OnException(std::string_view message) 
	{
		std::string_view processedMessage = message.substr(0, message.find_first_of('\n'));
		TR_CORE_ERROR(TR_LOG_SCRIPT, processedMessage);
		Log(processedMessage, spdlog::level::err);
	}

	static void OnMessage(std::string_view message, Coral::MessageLevel messageLevel) 
	{
		switch (messageLevel)
		{
		case Coral::MessageLevel::Info:		TR_CORE_INFO(TR_LOG_SCRIPT, message); break;
		case Coral::MessageLevel::Warning:	TR_CORE_WARN(TR_LOG_SCRIPT, message); break;
		case Coral::MessageLevel::Error:	TR_CORE_ERROR(TR_LOG_SCRIPT, message); break;
		default: ;
		}
	}

	static void CreateAssemblyLoadContext() 
	{
		s_Data->LoadContext = s_Data->HostInstance.CreateAssemblyLoadContext("ScriptAppContext");
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
		CreateAssemblyLoadContext();

		LoadCoreAssembly();
		TR_CORE_INFO(TR_LOG_SCRIPT, "Initialized script engine");
	}

	bool ScriptEngine::LoadCoreAssembly()
	{
		auto& coreAssembly = s_Data->Assemblies.at(TR_CORE_ASSEMBLY_INDEX);
		TR_CORE_INFO(TR_LOG_SCRIPT, "Loading core assembly at {}", s_Data->ScriptCoreAssemblyPath);
		coreAssembly = s_Data->LoadContext.LoadAssembly(s_Data->ScriptCoreAssemblyPath.string());
		TR_ASSERT(coreAssembly.GetLoadStatus() == Coral::AssemblyLoadStatus::Success, "Couldn't load the TerranScriptCore assembly");

		ScriptTypes::Initialize();
		s_Data->EntityIDFieldHandle = ScriptTypes::EntityType->GetField("m_Handle");
		TR_ASSERT(s_Data->EntityIDFieldHandle > -1, "Failed to find Terran.Entity m_ID field");

		InitializeTypeConverters();
		ScriptBindings::Bind(coreAssembly);
		return true;
	}

	void ScriptEngine::Shutdown()
	{
		s_Data->ScriptInstanceMap.clear();
		Coral::GC::Collect();
		s_Data->HostInstance.UnloadAssemblyLoadContext(s_Data->LoadContext);
		TR_CORE_INFO(TR_LOG_SCRIPT, "Shutdown script system");
	}

	void ScriptEngine::ReloadAppAssembly()
	{
		TR_CORE_INFO(TR_LOG_SCRIPT, "Reloading app assembly");
		std::unordered_map<UUID, std::unordered_map<UUID, std::unordered_map<std::string, Utils::Variant>>> scriptFieldsStates;
		std::unordered_map<UUID, std::unordered_map<UUID, std::unordered_map<std::string, std::vector<Utils::Variant>>>> scriptFieldArraysStates;

		for (const auto& [sceneId, entityScriptMap] : s_Data->ScriptInstanceMap)
		{
			auto scene = SceneManager::GetScene(sceneId);
			if (!scene)
				continue;

			for (const auto& [entityId, scriptInstance] : entityScriptMap) 
			{
				Entity entity = scene->FindEntityWithUUID(entityId);
				auto& sc = entity.GetComponent<ScriptComponent>();

				for (const auto& fieldHandle : sc.FieldHandles)
				{
					const ScriptField& field = scriptInstance->GetScriptField(fieldHandle);
					if (field.IsArray)
					{
						ScriptArray array = scriptInstance->GetScriptArray(fieldHandle);
						if (array.Rank > 1)
							continue;
						int32_t arrayLength = scriptInstance->GetFieldArrayLength(array);
						scriptFieldArraysStates[sceneId][entityId][field.Name].reserve(arrayLength);

						for (int32_t i = 0; i < arrayLength; i++)
							scriptFieldArraysStates.at(sceneId).at(entityId).at(field.Name).push_back(scriptInstance->GetFieldArrayValue<Utils::Variant>(array, i));

						continue;
					}

					scriptFieldsStates[sceneId][entityId][field.Name] = scriptInstance->GetFieldValue<Utils::Variant>(fieldHandle);
				}
			}
		}

		ScriptBindings::Unbind();
		Shutdown();
		CreateAssemblyLoadContext();

		bool coreAssemblyLoaded = LoadCoreAssembly();
		TR_ASSERT(coreAssemblyLoaded, "Couldn't load the core assembly");

		bool appAssemblyLoaded = LoadAppAssembly();
		if (!appAssemblyLoaded)
			return;

		auto& scenes = SceneManager::GetActiveScenes();

		for (const auto& [sceneId, scene] : scenes)
		{
			auto scriptComponentView = scene->GetEntitiesWith<ScriptComponent>();
			for (const auto e : scriptComponentView)
			{
				Entity entity(e, scene.get());
				auto& sc = entity.GetComponent<ScriptComponent>();
				const UUID& entityId = entity.GetID();
				Shared<ScriptInstance> instance = CreateScriptInstance(entity);

				if (!instance)
					continue;

				for (const auto& [fieldHandle, scriptField] : instance->m_Fields) 
				{
					if (scriptField.IsArray) 
					{
						ScriptArray array = instance->GetScriptArray(fieldHandle);
						if (array.Rank > 1)
							continue;

						try 
						{
							const std::vector<Utils::Variant>& cachedFieldArrayData = scriptFieldArraysStates.at(sceneId).at(entityId).at(scriptField.Name);

							if (instance->GetFieldArrayLength(array) != cachedFieldArrayData.size())
								instance->ResizeFieldArray(array, static_cast<int32_t>(cachedFieldArrayData.size()));

							for (size_t i = 0; i < cachedFieldArrayData.size(); i++)
								instance->SetFieldArrayValue(array, cachedFieldArrayData.at(i), static_cast<int32_t>(i));

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
						const Utils::Variant& cachedFieldData = scriptFieldsStates.at(sceneId).at(entityId).at(scriptField.Name);
						instance->SetFieldValue(fieldHandle, cachedFieldData);
					}
					catch (std::out_of_range e) 
					{
						// the scene, entity or field doesn't exist in the cached array fields' values
						continue;
					}
				}
			}
		}

		TR_CORE_INFO(TR_LOG_SCRIPT, "Reloaded assemblies!");
		Log("Reloaded assemblies", spdlog::level::info);
	}

	static ScriptFieldType GetScriptType(const Coral::Type& type)
	{
		Coral::ManagedType managedType = type.GetManagedType();
		if (managedType != Coral::ManagedType::Unknown && managedType != Coral::ManagedType::Pointer)
			return static_cast<ScriptFieldType>(managedType);

		if (s_Data->TypeConverters.contains(type.GetTypeId()))
			return s_Data->TypeConverters.at(type.GetTypeId());

		return ScriptFieldType::None;
	}

#define ADD_SYSTEM_TYPE(TypeName, Type)\
	typeConverters.emplace(typeCache.GetTypeByName("System."#TypeName)->GetTypeId(), ScriptFieldType::Type);

#define ADD_TERRAN_TYPE(TypeName)\
	typeConverters.emplace(typeCache.GetTypeByName("Terran."#TypeName)->GetTypeId(), ScriptFieldType::TypeName);

	void ScriptEngine::InitializeTypeConverters()
	{
		TR_CORE_INFO(TR_LOG_SCRIPT, "Initializing type converters");
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

	Shared<ScriptInstance> ScriptEngine::GetScriptInstance(Entity entity)
	{
		return GetScriptInstance(entity.GetSceneId(), entity.GetID());
	}

	Shared<ScriptInstance> ScriptEngine::GetScriptInstance(const UUID& sceneID, const UUID& entityID)
	{
		try 
		{
			return s_Data->ScriptInstanceMap.at(sceneID).at(entityID);
		}
		catch (std::out_of_range e) 
		{
			return nullptr;
		}
	}

	Shared<ScriptInstance> ScriptEngine::CreateScriptInstance(Entity entity)
	{
		TR_PROFILE_FUNCTION();
		auto& scriptComponent = entity.GetComponent<ScriptComponent>();

		if (scriptComponent.ModuleName.empty()) return nullptr;

		Coral::ManagedAssembly& appAssembly = s_Data->Assemblies.at(TR_APP_ASSEMBLY_INDEX);
		Coral::Type& type = appAssembly.GetType(scriptComponent.ModuleName);

		if (!type)
		{
			scriptComponent.ClassExists = false;
			TR_CORE_ERROR(TR_LOG_SCRIPT, "Class {0} doesn't exist", scriptComponent.ModuleName);
			return nullptr;
		}

		scriptComponent.ClassExists = true;
		
		if (s_Data->ScriptInstanceMap.contains(entity.GetSceneId()))
		{
			auto obj = s_Data->ScriptInstanceMap.at(entity.GetSceneId()).find(entity.GetID());
			if (obj != s_Data->ScriptInstanceMap.at(entity.GetSceneId()).end())
				return ((*obj).second);
		}

		if (!type.IsSubclassOf(*ScriptTypes::ScriptableType))
		{
			TR_CORE_ERROR(TR_LOG_SCRIPT, "Class {0} doesn not extend Scriptable", scriptComponent.ModuleName);
			return nullptr;
		}

		Shared<ScriptInstance> instance =
			s_Data->ScriptInstanceMap[entity.GetSceneId()][entity.GetID()] =
			CreateShared<ScriptInstance>(type, entity.GetID());

		scriptComponent.FieldHandles.clear();
		for (Coral::FieldInfo& fieldInfo : type.GetFields())
		{
			if (fieldInfo.GetAccessibility() == Coral::TypeAccessibility::Public
				|| fieldInfo.HasAttribute(*ScriptTypes::SerializeFieldType))
			{
				scriptComponent.FieldHandles.emplace_back(fieldInfo.GetHandle());
				Coral::ScopedString fieldName = fieldInfo.GetName();
				ScriptField field;
				Coral::Type& fieldType = fieldInfo.GetType();
				field.IsArray = fieldType.IsArray();
				field.Type = GetScriptType(field.IsArray ? fieldType.GetElementType() : fieldType);

				field.Name = fieldName;
				instance->m_Fields.emplace(fieldInfo.GetHandle(), field);
			}
		}
		
		return s_Data->ScriptInstanceMap.at(entity.GetSceneId()).at(entity.GetID());
	}

	void ScriptEngine::DestroyScriptInstance(Entity entity) 
	{
		if (!entity || !entity.HasComponent<TagComponent>())
			return;

		if (s_Data->ScriptInstanceMap.contains(entity.GetSceneId()) && s_Data->ScriptInstanceMap.at(entity.GetSceneId()).contains(entity.GetID()))
		{
			s_Data->ScriptInstanceMap[entity.GetSceneId()].erase(entity.GetID());

			if (s_Data->ScriptInstanceMap.empty())
				s_Data->ScriptInstanceMap.erase(entity.GetSceneId());
		}
	}

	void ScriptEngine::OnStart(Entity entity) 
	{
		TR_PROFILE_FUNCTION();
		Shared<ScriptInstance> instance = GetScriptInstance(entity);

		instance->InvokeInit();
	}

	void ScriptEngine::OnUpdate(Entity entity, float deltaTime) 
	{
		TR_PROFILE_FUNCTION();
		Shared<ScriptInstance> instance = GetScriptInstance(entity);

		instance->InvokeUpdate(deltaTime);
	}

	void ScriptEngine::OnPhysicsBeginContact(Entity collider, Entity collidee) 
	{
		TR_PROFILE_FUNCTION();
		if(Shared<ScriptInstance> instance = GetScriptInstance(collider))
			instance->InvokeCollisionBegin(collidee);
	}

	void ScriptEngine::OnPhysicsEndContact(Entity collider, Entity collidee) 
	{
		TR_PROFILE_FUNCTION();

		if(Shared<ScriptInstance> instance = GetScriptInstance(collider))
			instance->InvokeCollisionEnd(collidee);
	}

	void ScriptEngine::OnPhysicsUpdate(Entity entity) 
	{
		TR_PROFILE_FUNCTION();
		Shared<ScriptInstance> instance = GetScriptInstance(entity);
		instance->InvokePhysicsUpdate();
	}

	const void* ScriptEngine::CreateEntityInstance(const UUID& id) 
	{
		return ScriptTypes::EntityType->CreateInstance(id).GetHandle();
	}

	int32_t ScriptEngine::GetEntityIDFieldHandle()
	{
		return s_Data->EntityIDFieldHandle;
	}

	const void* ScriptEngine::CreateComponentInstance(int32_t componentTypeId, const UUID& entityId)
	{
		Coral::Type* componentType = Coral::TypeCache::Get().GetTypeByID(componentTypeId);
		TR_ASSERT(componentType, "Couldn't find component");

		return componentType->CreateInstance(entityId).GetHandle();
	}

	bool ScriptEngine::LoadAppAssembly()
	{
		auto& appAssembly = s_Data->Assemblies.at(TR_APP_ASSEMBLY_INDEX);
		TR_CORE_INFO(TR_LOG_SCRIPT, "Loading app assembly at: {}", Project::GetAppAssemblyPath());
		appAssembly = s_Data->LoadContext.LoadAssembly(Project::GetAppAssemblyPath().string());

		Coral::AssemblyLoadStatus status = appAssembly.GetLoadStatus();

		if (status != Coral::AssemblyLoadStatus::Success)
		{
			TR_CORE_ERROR(TR_LOG_SCRIPT, "Couldn't load the ScriptAssembly assembly");
			return false;
		}

		for (const auto type : appAssembly.GetTypes())
		{
			TR_CORE_TRACE(TR_LOG_SCRIPT, std::string(type->GetFullName()));
			TR_CORE_TRACE(TR_LOG_SCRIPT, std::string(type->GetAssemblyQualifiedName()));
		}

		return true;
	}
}
