#pragma once

#include "ScriptInstance.h"

#include <Coral/FieldInfo.hpp>
#include <Coral/Type.hpp>
#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/Layer.h>
#include <LibCore/Result.h>
#include <LibCore/UUID.h>

#include <LibScene/Entity.h>
#include <LibScene/Scene.h>

#include <LibScript/ManagedTypesCache.h>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <unordered_map>

#include <Coral/GC.hpp>
#include <Coral/HostInstance.hpp>
#include <Coral/TypeCache.hpp>

namespace Terran::Script {

#define TR_CORE_ASSEMBLY_INDEX 0
#define TR_APP_ASSEMBLY_INDEX 1
#define TR_ASSEMBLIES ((TR_APP_ASSEMBLY_INDEX) + 1)

class ScriptEngine final : public Terran::Core::Layer {
public:
    using script_instance_container_type = std::unordered_map<Core::UUID, std::unordered_map<Core::UUID, Core::Shared<ScriptInstance>>>;
    using ScriptTypeFilterFn = std::function<bool()>;

    ScriptEngine(Core::EventDispatcher& event_dispatcher, std::filesystem::path const& scriptCoreAssemblyPath);
    virtual ~ScriptEngine() override;

    virtual Core::Result<void> on_attach() override;

    void ReloadAppAssembly();

    Core::Shared<ScriptInstance> script_instance(World::Entity entity);

    Core::Shared<ScriptInstance> script_instance(Core::UUID const& sceneID, Core::UUID const& entityID);
    Core::Shared<ScriptInstance> create_script_instance(World::Entity entity);
    void destroy_script_instance(World::Entity entity);

    void on_start(World::Entity entity);
    void on_update(World::Entity entity, float deltaTime);

    // static void OnPhysicsBeginContact(Entity collider, Entity collidee);
    // static void OnPhysicsEndContact(Entity collider, Entity collidee);
    // static void OnPhysicsUpdate(Entity entity);

    void const* create_entity_type_instance(Core::UUID const& id);

    int32_t entity_id_field_handle() const
    {
        return m_entity_id_field_handle;
    }

    static void const* create_component_instance(int32_t componentTypeId, Core::UUID const& entityId);

    bool load_app_assembly(std::filesystem::path const& assembly_path);

private:
    void CreateAssemblyLoadContext();
    bool LoadCoreAssembly();
    bool intialize_type_map();
    void DispatchLog(std::string_view message, spdlog::level::level_enum logLevel);
    void OnException(std::string_view message);
    ScriptFieldType field_type(Coral::Type const& type);
    bool is_field_accessible(Coral::FieldInfo& field);
    bool is_type_scriptable_type(Coral::Type& type);

private:
    Coral::HostInstance m_host_instance;
    Coral::AssemblyLoadContext m_load_context;
    std::array<Coral::ManagedAssembly, TR_ASSEMBLIES> m_assemblies;

    std::string CoralDirectory = "Resources/Scripts";
    int32_t m_entity_id_field_handle = 0;

    script_instance_container_type script_instance_map;
    std::filesystem::path m_core_assembly_path;
    std::unordered_map<Coral::TypeId, ScriptFieldType> m_field_type_map;
    std::unordered_map<std::string, Coral::Type> m_type_map;
    ManagedTypesCache m_managed_types_cache;

    friend class ScriptBindings;
    friend class LogBinding;
};

}
