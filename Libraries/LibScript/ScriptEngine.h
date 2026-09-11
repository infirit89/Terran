#pragma once

#include "ScriptInstance.h"

#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/Layer.h>
#include <LibCore/UUID.h>

#include <LibScene/Entity.h>
#include <LibScene/Scene.h>

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

    void ReloadAppAssembly();

    Core::Shared<ScriptInstance> GetScriptInstance(World::Entity entity);
    Core::Shared<ScriptInstance> GetScriptInstance(Core::UUID const& sceneID, Core::UUID const& entityID);
    Core::Shared<ScriptInstance> CreateScriptInstance(World::Entity entity);
    void DestroyScriptInstance(World::Entity entity);

    void OnStart(World::Entity entity);
    void OnUpdate(World::Entity entity, float deltaTime);

    // static void OnPhysicsBeginContact(Entity collider, Entity collidee);
    // static void OnPhysicsEndContact(Entity collider, Entity collidee);
    // static void OnPhysicsUpdate(Entity entity);

    void const* CreateEntityInstance(Core::UUID const& id);
    int32_t GetEntityIDFieldHandle();

    void const* CreateComponentInstance(int32_t componentTypeId, Core::UUID const& entityId);

    bool LoadAppAssembly();

private:
    bool LoadCoreAssembly();
    void InitializeTypeConverters();

private:
    Coral::HostInstance HostInstance;
    Coral::AssemblyLoadContext LoadContext;
    std::array<Coral::ManagedAssembly, TR_ASSEMBLIES> Assemblies;

    std::string CoralDirectory = "Resources/Scripts";
    int32_t EntityIDFieldHandle = 0;

    script_instance_container_type ScriptInstanceMap;
    std::filesystem::path ScriptCoreAssemblyPath;
    std::unordered_map<Coral::TypeId, ScriptFieldType> TypeConverters;
    std::unordered_map<std::string, Coral::Type> ScriptTypeMap;

    friend class ScriptBindings;
};

}
