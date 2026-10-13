#include "ScriptEngine.h"

#include "ManagedTypesCache.h"
#include "ScriptBindings.h"
#include "ScriptComponent.h"
#include "ScriptEngineEvent.h"
#include "ScriptFieldType.h"
#include "ScriptInstance.h"
#include "ScriptTypes.h"
#include "Variant.h"

#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/Layer.h>
#include <LibCore/Log.h>
#include <LibCore/Result.h>
#include <LibCore/UUID.h>
#include <LibCore/Assert.h>

#include <LibScene/Components.h>
#include <LibScene/Entity.h>
#include <LibScene/SceneManager.h>
// #include "Project/Project.h"

// #include "Utils/Debug/OptickProfiler.h"

#include <Coral/GC.hpp>
#include <Coral/HostInstance.hpp>
#include <Coral/TypeCache.hpp>
#include <functional>

#include <filesystem>
#include <stdexcept>
#include <string_view>
#include <string>

namespace Terran::Script {

void ScriptEngine::DispatchLog(std::string_view message, spdlog::level::level_enum logLevel)
{
    ScriptEngineLogEvent logEvent(message, logLevel);
    event_dispatcher.trigger(logEvent);
}

void ScriptEngine::OnException(std::string_view message)
{
    std::string_view processedMessage = message.substr(0, message.find_first_of('\n'));
    TR_ERROR(SCRIPT_SYSTEM, processedMessage);
    DispatchLog(processedMessage, spdlog::level::err);
}

static void OnMessage(std::string_view message, Coral::MessageLevel messageLevel)
{
    switch (messageLevel) {
    case Coral::MessageLevel::Info:
        TR_INFO(SCRIPT_SYSTEM, message);
        break;
    case Coral::MessageLevel::Warning:
        TR_WARN(SCRIPT_SYSTEM, message);
        break;
    case Coral::MessageLevel::Error:
        TR_ERROR(SCRIPT_SYSTEM, message);
        break;
    default:;
    }
}

void ScriptEngine::CreateAssemblyLoadContext()
{
    m_load_context = m_host_instance.CreateAssemblyLoadContext("ScriptAppContext");
}

ScriptEngine::ScriptEngine(Core::EventDispatcher& event_dispatcher, std::filesystem::path const& scriptCoreAssemblyPath)
    : Core::Layer(SCRIPT_SYSTEM, event_dispatcher)
    , m_core_assembly_path(scriptCoreAssemblyPath)
{
}

Core::Result<void> ScriptEngine::on_attach()
{
    Coral::HostSettings settings = {
        .CoralDirectory = CoralDirectory,
        .MessageCallback = OnMessage,
        .ExceptionCallback = std::bind_front(&ScriptEngine::OnException, this),
    };

    if (!m_host_instance.Initialize(settings)) {
        TR_ERROR(SCRIPT_SYSTEM, "Failed to initialize host instance");
        return Core::Bad();
    }

    CreateAssemblyLoadContext();

    LoadCoreAssembly();
    TR_INFO(SCRIPT_SYSTEM, "Initialized script engine");

    return {};
}

bool ScriptEngine::LoadCoreAssembly()
{
    auto& coreAssembly = m_assemblies.at(TR_CORE_ASSEMBLY_INDEX);
    TR_INFO(SCRIPT_SYSTEM, "Loading core assembly at {}", m_core_assembly_path);
    coreAssembly = m_load_context.LoadAssembly(m_core_assembly_path.string());
    if (coreAssembly.GetLoadStatus() != Coral::AssemblyLoadStatus::Success) {
        return false;
    }

    auto managed_types_cache_res = ManagedTypesCache::initialize_cache();

    if (!managed_types_cache_res.is_ok()) {
        TR_ERROR(SCRIPT_SYSTEM, managed_types_cache_res.error().message());
        return false;
    }

    m_managed_types_cache = managed_types_cache_res.value();
    m_entity_id_field_handle = m_managed_types_cache.entity_type()->GetField("m_Handle");
    if (m_entity_id_field_handle <= -1) {
        TR_ERROR(SCRIPT_SYSTEM, "Failed to find Terran.Entity m_ID field");
        return false;
    }

    intialize_type_map();
    // ScriptBindings::Bind(coreAssembly);
    return true;
}

ScriptEngine::~ScriptEngine()
{
    script_instance_map.clear();
    Coral::GC::Collect();
    m_host_instance.UnloadAssemblyLoadContext(m_load_context);
    TR_INFO(SCRIPT_SYSTEM, "Shutdown script system");
}

void ScriptEngine::ReloadAppAssembly()
{
    // TR_CORE_INFO(TR_LOG_SCRIPT, "Reloading app assembly");
    // std::unordered_map<Terran::Core::UUID, std::unordered_map<Terran::Core::UUID, std::unordered_map<std::string, Utils::Variant>>> scriptFieldsStates;
    // std::unordered_map<Terran::Core::UUID, std::unordered_map<Terran::Core::UUID, std::unordered_map<std::string, std::vector<Utils::Variant>>>> scriptFieldArraysStates;
    //
    // for (auto const& [sceneId, entityScriptMap] : ScriptInstanceMap) {
    //     auto scene = SceneManager::GetScene(sceneId);
    //     if (!scene)
    //         continue;
    //
    //     for (auto const& [entityId, scriptInstance] : entityScriptMap) {
    //         Entity entity = scene->FindEntityWithUUID(entityId);
    //         auto& sc = entity.GetComponent<ScriptComponent>();
    //
    //         for (auto const& fieldHandle : sc.FieldHandles) {
    //             ScriptField const& field = scriptInstance->GetScriptField(fieldHandle);
    //             if (field.IsArray) {
    //                 ScriptArray array = scriptInstance->GetScriptArray(fieldHandle);
    //                 if (array.Rank > 1)
    //                     continue;
    //                 int32_t arrayLength = scriptInstance->GetFieldArrayLength(array);
    //                 scriptFieldArraysStates[sceneId][entityId][field.Name].reserve(arrayLength);
    //
    //                 for (int32_t i = 0; i < arrayLength; i++)
    //                     scriptFieldArraysStates.at(sceneId).at(entityId).at(field.Name).push_back(scriptInstance->GetFieldArrayValue<Utils::Variant>(array, i));
    //
    //                 continue;
    //             }
    //
    //             scriptFieldsStates[sceneId][entityId][field.Name] = scriptInstance->GetFieldValue<Utils::Variant>(fieldHandle);
    //         }
    //     }
    // }
    //
    // ScriptBindings::Unbind();
    // Shutdown();
    // CreateAssemblyLoadContext();
    //
    // bool coreAssemblyLoaded = LoadCoreAssembly();
    // TR_ASSERT(coreAssemblyLoaded, "Couldn't load the core assembly");
    //
    // bool appAssemblyLoaded = LoadAppAssembly();
    // if (!appAssemblyLoaded)
    //     return;
    //
    // auto& scenes = SceneManager::GetActiveScenes();
    //
    // for (auto const& [sceneId, scene] : scenes) {
    //     auto scriptComponentView = scene->GetEntitiesWith<ScriptComponent>();
    //     for (auto const e : scriptComponentView) {
    //         Entity entity(e, scene.get());
    //         auto& sc = entity.GetComponent<ScriptComponent>();
    //         Terran::Core::UUID const& entityId = entity.GetID();
    //         Terran::Core::Shared<ScriptInstance> instance = CreateScriptInstance(entity);
    //
    //         if (!instance)
    //             continue;
    //
    //         for (auto const& [fieldHandle, scriptField] : instance->m_Fields) {
    //             if (scriptField.IsArray) {
    //                 ScriptArray array = instance->GetScriptArray(fieldHandle);
    //                 if (array.Rank > 1)
    //                     continue;
    //
    //                 try {
    //                     std::vector<Utils::Variant> const& cachedFieldArrayData = scriptFieldArraysStates.at(sceneId).at(entityId).at(scriptField.Name);
    //
    //                     if (instance->GetFieldArrayLength(array) != cachedFieldArrayData.size())
    //                         instance->ResizeFieldArray(array, static_cast<int32_t>(cachedFieldArrayData.size()));
    //
    //                     for (size_t i = 0; i < cachedFieldArrayData.size(); i++)
    //                         instance->SetFieldArrayValue(array, cachedFieldArrayData.at(i), static_cast<int32_t>(i));
    //
    //                     continue;
    //                 } catch (std::out_of_range e) {
    //                     // the scene, entity or field doesn't exist in the cached array fields' values
    //                     continue;
    //                 }
    //             }
    //
    //             try {
    //                 Utils::Variant const& cachedFieldData = scriptFieldsStates.at(sceneId).at(entityId).at(scriptField.Name);
    //                 instance->SetFieldValue(fieldHandle, cachedFieldData);
    //             } catch (std::out_of_range e) {
    //                 // the scene, entity or field doesn't exist in the cached array fields' values
    //                 continue;
    //             }
    //         }
    //     }
    // }
    //
    // TR_CORE_INFO(TR_LOG_SCRIPT, "Reloaded assemblies!");
    // Log("Reloaded assemblies", spdlog::level::info);
}

ScriptFieldType ScriptEngine::field_type(Coral::Type const& type)
{
    Coral::ManagedType managedType = type.GetManagedType();
    if (managedType != Coral::ManagedType::Unknown && managedType != Coral::ManagedType::Pointer)
        return static_cast<ScriptFieldType>(managedType);

    if (m_field_type_map.contains(type.GetTypeId()))
        return m_field_type_map.at(type.GetTypeId());

    return ScriptFieldType::None;
}

#define ADD_SYSTEM_TYPE(TypeName, Type) \
    typeConverters.emplace(typeCache.GetTypeByName("System." #TypeName)->GetTypeId(), ScriptFieldType::Type);

#define ADD_TERRAN_TYPE(TypeName)                                             \
    auto type_##TypeName = typeCache.GetTypeByName("Terran." #TypeName);      \
    if (!type_##TypeName) {                                                   \
        TR_ERROR(SCRIPT_SYSTEM, "Failed to find type: Terran.{}", #TypeName); \
        return false;                                                         \
    }                                                                         \
    m_field_type_map.emplace(type_##TypeName->GetTypeId(), ScriptFieldType::TypeName)

bool ScriptEngine::intialize_type_map()
{
    TR_TRACE(SCRIPT_SYSTEM, "Initializing type map");
    auto& typeCache = Coral::TypeCache::Get();
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
    return true;
}

Core::Shared<ScriptInstance> ScriptEngine::script_instance(World::Entity entity)
{
    return script_instance(entity.scene_id(), entity.id());
}

Core::Shared<ScriptInstance> ScriptEngine::script_instance(Core::UUID const& sceneID, Core::UUID const& entityID)
{
    try {
        return script_instance_map.at(sceneID).at(entityID);
    } catch (std::out_of_range const& e) {
        return nullptr;
    }
}

bool ScriptEngine::is_field_accessible(Coral::FieldInfo& field)
{
    return field.GetAccessibility() == Coral::TypeAccessibility::Public
        || field.HasAttribute(*m_managed_types_cache.serialize_field_type());
}

bool ScriptEngine::is_type_scriptable_type(Coral::Type& type)
{
    return type.IsSubclassOf(*m_managed_types_cache.scriptable_type());
}

Core::Shared<ScriptInstance> ScriptEngine::create_script_instance(World::Entity entity)
{
    // TR_PROFILE_FUNCTION();
    auto& scriptComponent = entity.get_component<ScriptComponent>();

    if (scriptComponent.ModuleName.empty())
        return nullptr;

    Coral::ManagedAssembly& appAssembly = m_assemblies.at(TR_APP_ASSEMBLY_INDEX);
    Coral::Type& type = appAssembly.GetType(scriptComponent.ModuleName);

    if (!type) {
        scriptComponent.ClassExists = false;
        TR_ERROR(SCRIPT_SYSTEM, "Class {0} doesn't exist", scriptComponent.ModuleName);
        return nullptr;
    }

    scriptComponent.ClassExists = true;

    if (script_instance_map.contains(entity.scene_id())) {
        auto obj = script_instance_map.at(entity.scene_id()).find(entity.id());
        if (obj != script_instance_map.at(entity.scene_id()).end())
            return ((*obj).second);
    }

    if (!is_type_scriptable_type(type)) {
        TR_ERROR(SCRIPT_SYSTEM, "Class {0} doesn not extend Scriptable", scriptComponent.ModuleName);
        return nullptr;
    }

    Core::Shared<ScriptInstance> instance
        = script_instance_map[entity.scene_id()][entity.id()]
        = Core::CreateShared<ScriptInstance>(this, type, entity.id());

    scriptComponent.FieldHandles.clear();
    for (Coral::FieldInfo& fieldInfo : type.GetFields()) {

        if (!is_field_accessible(fieldInfo)) {
            continue;
        }

        scriptComponent.FieldHandles.emplace_back(fieldInfo.GetHandle());
        Coral::ScopedString fieldName = fieldInfo.GetName();
        ScriptField field;
        Coral::Type& fieldType = fieldInfo.GetType();
        field.IsArray = fieldType.IsArray();
        field.Type = field_type(field.IsArray ? fieldType.GetElementType() : fieldType);

        field.Name = fieldName;
        instance->m_fields.emplace(fieldInfo.GetHandle(), field);
    }

    return script_instance_map.at(entity.scene_id()).at(entity.id());
}

void ScriptEngine::destroy_script_instance(World::Entity entity)
{
    if (!entity || !entity.has_component<World::TagComponent>())
        return;

    if (script_instance_map.contains(entity.scene_id()) && script_instance_map.at(entity.scene_id()).contains(entity.id())) {
        script_instance_map[entity.scene_id()].erase(entity.id());

        if (script_instance_map.empty())
            script_instance_map.erase(entity.scene_id());
    }
}

void ScriptEngine::on_start(World::Entity entity)
{
    // TR_PROFILE_FUNCTION();
    Core::Shared<ScriptInstance> instance = script_instance(entity);

    instance->invoke_init();
}

void ScriptEngine::on_update(World::Entity entity, float deltaTime)
{
    // TR_PROFILE_FUNCTION();
    Core::Shared<ScriptInstance> instance = script_instance(entity);

    instance->invoke_update(deltaTime);
}

// void ScriptEngine::OnPhysicsBeginContact(Entity collider, Entity collidee)
// {
//     TR_PROFILE_FUNCTION();
//     if (Terran::Core::Shared<ScriptInstance> instance = GetScriptInstance(collider))
//         instance->InvokeCollisionBegin(collidee);
// }
//
// void ScriptEngine::OnPhysicsEndContact(Entity collider, Entity collidee)
// {
//     TR_PROFILE_FUNCTION();
//
//     if (Terran::Core::Shared<ScriptInstance> instance = GetScriptInstance(collider))
//         instance->InvokeCollisionEnd(collidee);
// }
//
// void ScriptEngine::OnPhysicsUpdate(Entity entity)
// {
//     TR_PROFILE_FUNCTION();
//     Terran::Core::Shared<ScriptInstance> instance = GetScriptInstance(entity);
//     instance->InvokePhysicsUpdate();
// }

void const* ScriptEngine::create_entity_type_instance(Core::UUID const& id)
{
    return m_managed_types_cache.entity_type()->CreateInstance(id).GetHandle();
}

void const* ScriptEngine::create_component_instance(int32_t componentTypeId, Core::UUID const& entityId)
{
    Coral::Type* componentType = Coral::TypeCache::Get().GetTypeByID(componentTypeId);
    TR_ASSERT(componentType, "Couldn't find component");

    return componentType->CreateInstance(entityId).GetHandle();
}

bool ScriptEngine::load_app_assembly(std::filesystem::path const& assembly_path)
{
    auto& appAssembly = m_assemblies.at(TR_APP_ASSEMBLY_INDEX);
    TR_INFO(SCRIPT_SYSTEM, "Loading app assembly at: {}", assembly_path);
    appAssembly = m_load_context.LoadAssembly(assembly_path.string());

    Coral::AssemblyLoadStatus status = appAssembly.GetLoadStatus();

    if (status != Coral::AssemblyLoadStatus::Success) {
        TR_ERROR(SCRIPT_SYSTEM, "Couldn't load the ScriptAssembly assembly");
        return false;
    }

    for (auto const& type : appAssembly.GetTypes()) {
        TR_TRACE(SCRIPT_SYSTEM, std::string(type->GetFullName()));
        TR_TRACE(SCRIPT_SYSTEM, std::string(type->GetAssemblyQualifiedName()));
    }

    return true;
}

}
