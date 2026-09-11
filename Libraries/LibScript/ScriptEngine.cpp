#include "ScriptEngine.h"

#include "ScriptBindings.h"
#include "ScriptTypes.h"
#include "ScriptEngineEvent.h"
#include "Variant.h"

#include <LibCore/Layer.h>
#include <LibCore/Log.h>

#include <LibScene/SceneManager.h>
// #include "Project/Project.h"


// #include "Utils/Debug/OptickProfiler.h"

#include <Coral/GC.hpp>
#include <Coral/HostInstance.hpp>
#include <Coral/TypeCache.hpp>

namespace Terran::Script {

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
    switch (messageLevel) {
    case Coral::MessageLevel::Info:
        TR_CORE_INFO(TR_LOG_SCRIPT, message);
        break;
    case Coral::MessageLevel::Warning:
        TR_CORE_WARN(TR_LOG_SCRIPT, message);
        break;
    case Coral::MessageLevel::Error:
        TR_CORE_ERROR(TR_LOG_SCRIPT, message);
        break;
    default:;
    }
}

static void CreateAssemblyLoadContext()
{
    LoadContext = HostInstance.CreateAssemblyLoadContext("ScriptAppContext");
}

ScriptEngine::ScriptEngine(Core::EventDispatcher& event_dispatcher, std::filesystem::path const& scriptCoreAssemblyPath)
    : Core::Layer(SCRIPT_SYSTEM, event_dispatcher)
{
    ScriptCoreAssemblyPath = scriptCoreAssemblyPath;

    Coral::HostSettings settings = {
        .CoralDirectory = CoralDirectory,
        .MessageCallback = OnMessage,
        .ExceptionCallback = OnException
    };

    HostInstance.Initialize(settings);
    CreateAssemblyLoadContext();

    LoadCoreAssembly();
    TR_INFO(SCRIPT_SYSTEM, "Initialized script engine");
}

bool ScriptEngine::LoadCoreAssembly()
{
    auto& coreAssembly = Assemblies.at(TR_CORE_ASSEMBLY_INDEX);
    TR_CORE_INFO(TR_LOG_SCRIPT, "Loading core assembly at {}", ScriptCoreAssemblyPath);
    coreAssembly = LoadContext.LoadAssembly(ScriptCoreAssemblyPath.string());
    TR_ASSERT(coreAssembly.GetLoadStatus() == Coral::AssemblyLoadStatus::Success, "Couldn't load the TerranScriptCore assembly");

    ScriptTypes::Initialize();
    EntityIDFieldHandle = ScriptTypes::EntityType->GetField("m_Handle");
    TR_ASSERT(EntityIDFieldHandle > -1, "Failed to find Terran.Entity m_ID field");

    InitializeTypeConverters();
    ScriptBindings::Bind(coreAssembly);
    return true;
}

ScriptEngine::~ScriptEngine()
{
    ScriptInstanceMap.clear();
    Coral::GC::Collect();
    HostInstance.UnloadAssemblyLoadContext(LoadContext);
    TR_INFO(SCRIPT_SYSTEM, "Shutdown script system");
}

void ScriptEngine::ReloadAppAssembly()
{
    TR_CORE_INFO(TR_LOG_SCRIPT, "Reloading app assembly");
    std::unordered_map<Terran::Core::UUID, std::unordered_map<Terran::Core::UUID, std::unordered_map<std::string, Utils::Variant>>> scriptFieldsStates;
    std::unordered_map<Terran::Core::UUID, std::unordered_map<Terran::Core::UUID, std::unordered_map<std::string, std::vector<Utils::Variant>>>> scriptFieldArraysStates;

    for (auto const& [sceneId, entityScriptMap] : ScriptInstanceMap) {
        auto scene = SceneManager::GetScene(sceneId);
        if (!scene)
            continue;

        for (auto const& [entityId, scriptInstance] : entityScriptMap) {
            Entity entity = scene->FindEntityWithUUID(entityId);
            auto& sc = entity.GetComponent<ScriptComponent>();

            for (auto const& fieldHandle : sc.FieldHandles) {
                ScriptField const& field = scriptInstance->GetScriptField(fieldHandle);
                if (field.IsArray) {
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

    for (auto const& [sceneId, scene] : scenes) {
        auto scriptComponentView = scene->GetEntitiesWith<ScriptComponent>();
        for (auto const e : scriptComponentView) {
            Entity entity(e, scene.get());
            auto& sc = entity.GetComponent<ScriptComponent>();
            Terran::Core::UUID const& entityId = entity.GetID();
            Terran::Core::Shared<ScriptInstance> instance = CreateScriptInstance(entity);

            if (!instance)
                continue;

            for (auto const& [fieldHandle, scriptField] : instance->m_Fields) {
                if (scriptField.IsArray) {
                    ScriptArray array = instance->GetScriptArray(fieldHandle);
                    if (array.Rank > 1)
                        continue;

                    try {
                        std::vector<Utils::Variant> const& cachedFieldArrayData = scriptFieldArraysStates.at(sceneId).at(entityId).at(scriptField.Name);

                        if (instance->GetFieldArrayLength(array) != cachedFieldArrayData.size())
                            instance->ResizeFieldArray(array, static_cast<int32_t>(cachedFieldArrayData.size()));

                        for (size_t i = 0; i < cachedFieldArrayData.size(); i++)
                            instance->SetFieldArrayValue(array, cachedFieldArrayData.at(i), static_cast<int32_t>(i));

                        continue;
                    } catch (std::out_of_range e) {
                        // the scene, entity or field doesn't exist in the cached array fields' values
                        continue;
                    }
                }

                try {
                    Utils::Variant const& cachedFieldData = scriptFieldsStates.at(sceneId).at(entityId).at(scriptField.Name);
                    instance->SetFieldValue(fieldHandle, cachedFieldData);
                } catch (std::out_of_range e) {
                    // the scene, entity or field doesn't exist in the cached array fields' values
                    continue;
                }
            }
        }
    }

    TR_CORE_INFO(TR_LOG_SCRIPT, "Reloaded assemblies!");
    Log("Reloaded assemblies", spdlog::level::info);
}

static ScriptFieldType GetScriptType(Coral::Type const& type)
{
    Coral::ManagedType managedType = type.GetManagedType();
    if (managedType != Coral::ManagedType::Unknown && managedType != Coral::ManagedType::Pointer)
        return static_cast<ScriptFieldType>(managedType);

    if (TypeConverters.contains(type.GetTypeId()))
        return TypeConverters.at(type.GetTypeId());

    return ScriptFieldType::None;
}

#define ADD_SYSTEM_TYPE(TypeName, Type) \
    typeConverters.emplace(typeCache.GetTypeByName("System." #TypeName)->GetTypeId(), ScriptFieldType::Type);

#define ADD_TERRAN_TYPE(TypeName) \
    typeConverters.emplace(typeCache.GetTypeByName("Terran." #TypeName)->GetTypeId(), ScriptFieldType::TypeName);

void ScriptEngine::InitializeTypeConverters()
{
    TR_CORE_INFO(TR_LOG_SCRIPT, "Initializing type converters");
    auto& typeCache = Coral::TypeCache::Get();
    auto& typeConverters = TypeConverters;
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

Terran::Core::Shared<ScriptInstance> ScriptEngine::GetScriptInstance(Entity entity)
{
    return GetScriptInstance(entity.GetSceneId(), entity.GetID());
}

Terran::Core::Shared<ScriptInstance> ScriptEngine::GetScriptInstance(Terran::Core::UUID const& sceneID, Terran::Core::UUID const& entityID)
{
    try {
        return ScriptInstanceMap.at(sceneID).at(entityID);
    } catch (std::out_of_range e) {
        return nullptr;
    }
}

Terran::Core::Shared<ScriptInstance> ScriptEngine::CreateScriptInstance(Entity entity)
{
    TR_PROFILE_FUNCTION();
    auto& scriptComponent = entity.GetComponent<ScriptComponent>();

    if (scriptComponent.ModuleName.empty())
        return nullptr;

    Coral::ManagedAssembly& appAssembly = Assemblies.at(TR_APP_ASSEMBLY_INDEX);
    Coral::Type& type = appAssembly.GetType(scriptComponent.ModuleName);

    if (!type) {
        scriptComponent.ClassExists = false;
        TR_CORE_ERROR(TR_LOG_SCRIPT, "Class {0} doesn't exist", scriptComponent.ModuleName);
        return nullptr;
    }

    scriptComponent.ClassExists = true;

    if (ScriptInstanceMap.contains(entity.GetSceneId())) {
        auto obj = ScriptInstanceMap.at(entity.GetSceneId()).find(entity.GetID());
        if (obj != ScriptInstanceMap.at(entity.GetSceneId()).end())
            return ((*obj).second);
    }

    if (!type.IsSubclassOf(*ScriptTypes::ScriptableType)) {
        TR_CORE_ERROR(TR_LOG_SCRIPT, "Class {0} doesn not extend Scriptable", scriptComponent.ModuleName);
        return nullptr;
    }

    Terran::Core::Shared<ScriptInstance> instance = ScriptInstanceMap[entity.GetSceneId()][entity.GetID()] = Terran::Core::CreateShared<ScriptInstance>(type, entity.GetID());

    scriptComponent.FieldHandles.clear();
    for (Coral::FieldInfo& fieldInfo : type.GetFields()) {
        if (fieldInfo.GetAccessibility() == Coral::TypeAccessibility::Public
            || fieldInfo.HasAttribute(*ScriptTypes::SerializeFieldType)) {
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

    return ScriptInstanceMap.at(entity.GetSceneId()).at(entity.GetID());
}

void ScriptEngine::DestroyScriptInstance(Entity entity)
{
    if (!entity || !entity.HasComponent<TagComponent>())
        return;

    if (ScriptInstanceMap.contains(entity.GetSceneId()) && ScriptInstanceMap.at(entity.GetSceneId()).contains(entity.GetID())) {
        ScriptInstanceMap[entity.GetSceneId()].erase(entity.GetID());

        if (ScriptInstanceMap.empty())
            ScriptInstanceMap.erase(entity.GetSceneId());
    }
}

void ScriptEngine::OnStart(Entity entity)
{
    TR_PROFILE_FUNCTION();
    Terran::Core::Shared<ScriptInstance> instance = GetScriptInstance(entity);

    instance->InvokeInit();
}

void ScriptEngine::OnUpdate(Entity entity, float deltaTime)
{
    TR_PROFILE_FUNCTION();
    Terran::Core::Shared<ScriptInstance> instance = GetScriptInstance(entity);

    instance->InvokeUpdate(deltaTime);
}

void ScriptEngine::OnPhysicsBeginContact(Entity collider, Entity collidee)
{
    TR_PROFILE_FUNCTION();
    if (Terran::Core::Shared<ScriptInstance> instance = GetScriptInstance(collider))
        instance->InvokeCollisionBegin(collidee);
}

void ScriptEngine::OnPhysicsEndContact(Entity collider, Entity collidee)
{
    TR_PROFILE_FUNCTION();

    if (Terran::Core::Shared<ScriptInstance> instance = GetScriptInstance(collider))
        instance->InvokeCollisionEnd(collidee);
}

void ScriptEngine::OnPhysicsUpdate(Entity entity)
{
    TR_PROFILE_FUNCTION();
    Terran::Core::Shared<ScriptInstance> instance = GetScriptInstance(entity);
    instance->InvokePhysicsUpdate();
}

void const* ScriptEngine::CreateEntityInstance(Terran::Core::UUID const& id)
{
    return ScriptTypes::EntityType->CreateInstance(id).GetHandle();
}

int32_t ScriptEngine::GetEntityIDFieldHandle()
{
    return EntityIDFieldHandle;
}

void const* ScriptEngine::CreateComponentInstance(int32_t componentTypeId, Terran::Core::UUID const& entityId)
{
    Coral::Type* componentType = Coral::TypeCache::Get().GetTypeByID(componentTypeId);
    TR_ASSERT(componentType, "Couldn't find component");

    return componentType->CreateInstance(entityId).GetHandle();
}

bool ScriptEngine::LoadAppAssembly()
{
    auto& appAssembly = Assemblies.at(TR_APP_ASSEMBLY_INDEX);
    TR_CORE_INFO(TR_LOG_SCRIPT, "Loading app assembly at: {}", Project::GetAppAssemblyPath());
    appAssembly = LoadContext.LoadAssembly(Project::GetAppAssemblyPath().string());

    Coral::AssemblyLoadStatus status = appAssembly.GetLoadStatus();

    if (status != Coral::AssemblyLoadStatus::Success) {
        TR_CORE_ERROR(TR_LOG_SCRIPT, "Couldn't load the ScriptAssembly assembly");
        return false;
    }

    for (auto const type : appAssembly.GetTypes()) {
        TR_CORE_TRACE(TR_LOG_SCRIPT, std::string(type->GetFullName()));
        TR_CORE_TRACE(TR_LOG_SCRIPT, std::string(type->GetAssemblyQualifiedName()));
    }

    return true;
}

}
