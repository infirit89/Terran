#include "trpch.h"
#include "ScriptEngine.h"

#include "ScriptBindings.h"

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
#include <Coral/ManagedArray.hpp>

#include <oaidl.h>

namespace TerranEngine
{
	using ScriptInstanceMap = std::unordered_map<UUID, std::unordered_map<UUID, Shared<ScriptInstance>>>;
	
	struct ScriptEngineData 
	{
		Coral::HostInstance HostInstance;
		Coral::AssemblyLoadContext LoadContext;
		std::array<Coral::ManagedAssembly, TR_ASSEMBLIES> Assemblies;
		
		std::string CoralDirectory = "Resources/Scripts";
		Coral::Type ScriptableBaseClass;
		Coral::Type EntityClass;
		int32_t EntityIDFieldHandle;

		ScriptInstanceMap ScriptInstanceMap;
        std::filesystem::path ScriptCoreAssemblyPath;
		std::function<void(std::string, spdlog::level::level_enum)> LogCallback;
		std::unordered_map<Coral::TypeId, ScriptType> TypeConverters;
	};

	static ScriptEngineData* s_Data;

	static void Log(const std::string& message, spdlog::level::level_enum logLevel) 
	{
		if (s_Data->LogCallback)
			s_Data->LogCallback(message, logLevel);
	}

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
	}

	bool ScriptEngine::LoadCoreAssembly() 
	{
		auto& coreAssembly = s_Data->Assemblies.at(TR_CORE_ASSEMBLY_INDEX);
		coreAssembly = s_Data->LoadContext.LoadAssembly(s_Data->ScriptCoreAssemblyPath.string());
		TR_ASSERT(coreAssembly.GetLoadStatus() == Coral::AssemblyLoadStatus::Success, "Couldn't load the TerranScriptCore assembly");

		s_Data->ScriptableBaseClass = coreAssembly.GetType("Terran.Scriptable");
		TR_ASSERT(s_Data->ScriptableBaseClass, "The scriptable base class wasn't found");

		s_Data->EntityClass = coreAssembly.GetType("Terran.Entity");
		TR_ASSERT(s_Data->EntityClass, "The entity class wasn't found");
		s_Data->EntityIDFieldHandle = s_Data->EntityClass.GetField("m_ID");

		InitializeTypeConverters();
		ScriptBindings::Bind(coreAssembly);
		return true;
	}

	void ScriptEngine::Shutdown()
	{
		Coral::GC::Collect();
		s_Data->HostInstance.UnloadAssemblyLoadContext(s_Data->LoadContext);
		s_Data->ScriptInstanceMap.clear();
	}

	void ScriptEngine::ReloadAppAssembly()
	{
		std::unordered_map<UUID, std::unordered_map<UUID, std::unordered_map<std::string, Utils::Variant>>> scriptFieldsStates;
		std::unordered_map<UUID, std::unordered_map<UUID, std::unordered_map<std::string, std::vector<Utils::Variant>>>> scriptFieldArraysStates;

		for (const auto& [sceneId, entityScriptMap] : s_Data->ScriptInstanceMap)
		{
			auto scene = SceneManager::GetScene(sceneId);
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

		for (const auto& [sceneId, scene] : SceneManager::GetActiveScenes())
		{
			auto scriptComponentView = scene->GetEntitiesWith<ScriptComponent>();
			for (const auto e : scriptComponentView)
			{
				Entity entity(e, scene.get());
				auto& sc = entity.GetComponent<ScriptComponent>();
				const UUID& entityId = entity.GetID();
				Shared<ScriptInstance> instance = InitializeScriptable(entity);

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
								instance->ResizeFieldArray(array, (int32_t)cachedFieldArrayData.size());

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

		TR_INFO("Reloaded assemblies!");
		Log("Reloaded assemblies", spdlog::level::info);
	}

	bool ScriptEngine::ClassExists(const std::string& moduleName) { return false;  }

	static ScriptType GetScriptType(Coral::Type& type)
	{
		Coral::ManagedType managedType = type.GetManagedType();
		if (managedType != Coral::ManagedType::Unknown && managedType != Coral::ManagedType::Pointer)
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

	Shared<ScriptInstance> ScriptEngine::GetScriptInstance(Entity entity)
	{
		return s_Data->ScriptInstanceMap.at(entity.GetSceneID()).at(entity.GetID());
	}

	Shared<ScriptInstance> ScriptEngine::GetScriptInstance(const UUID& sceneID, const UUID& entityID) 
	{
		return s_Data->ScriptInstanceMap.at(sceneID).at(entityID);
	}

	Shared<ScriptInstance> ScriptEngine::InitializeScriptable(Entity entity)
	{
		TR_PROFILE_FUNCTION();
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

		Shared<ScriptInstance> instance =
			s_Data->ScriptInstanceMap[entity.GetSceneID()][entity.GetID()] =
			CreateShared<ScriptInstance>(type, entity);

		scriptComponent.FieldHandles.clear();
		for (Coral::FieldInfo& fieldInfo : type.GetFields())
		{
			if (fieldInfo.GetAccessibility() == Coral::TypeAccessibility::Public)
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
		
		return s_Data->ScriptInstanceMap.at(entity.GetSceneID()).at(entity.GetID());
	}

	void ScriptEngine::UninitalizeScriptable(Entity entity) 
	{
		if (!entity || !entity.HasComponent<TagComponent>())
			return;

		if (s_Data->ScriptInstanceMap.contains(entity.GetSceneID()) && s_Data->ScriptInstanceMap.at(entity.GetSceneID()).contains(entity.GetID()))
		{
			s_Data->ScriptInstanceMap[entity.GetSceneID()].erase(entity.GetID());

			if (s_Data->ScriptInstanceMap.empty())
				s_Data->ScriptInstanceMap.erase(entity.GetSceneID());
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

	void ScriptEngine::OnPhysicsBeginContact(Entity collider, Entity collidee) {}
	void ScriptEngine::OnPhysicsEndContact(Entity collider, Entity collidee) {}

	void ScriptEngine::OnPhysicsUpdate(Entity entity) {}

	const void* ScriptEngine::CreateEntityInstance(const UUID& id) 
	{
		return s_Data->EntityClass.CreateInstance(id).GetHandle();
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
		appAssembly = s_Data->LoadContext.LoadAssembly(Project::GetAppAssemblyPath().string());

		Coral::AssemblyLoadStatus status = appAssembly.GetLoadStatus();

		if (status != Coral::AssemblyLoadStatus::Success)
		{
			TR_ERROR("Couldn't load the ScriptAssembly assembly");
			return false;
		}

		return true;
	}
}
