#pragma once

#include "ScriptInstance.h"

#include "LibCore/Base.h"
#include "LibCore/UUID.h"

#include "Scene/Scene.h"

#include <cstdint>
#include <filesystem>
#include <functional>

namespace TerranEngine {

#define TR_CORE_ASSEMBLY_INDEX 0
#define TR_APP_ASSEMBLY_INDEX 1
#define TR_ASSEMBLIES ((TR_APP_ASSEMBLY_INDEX) + 1)

class ScriptEngine final {
public:
    using ScriptTypeFilterFn = std::function<bool()>;
    static void Initialize(std::filesystem::path const& scriptCoreAssemblyPath);
    static void Shutdown();

    static void ReloadAppAssembly();

    static Terran::Core::Shared<ScriptInstance> GetScriptInstance(Entity entity);
    static Terran::Core::Shared<ScriptInstance> GetScriptInstance(Terran::Core::UUID const& sceneID, Terran::Core::UUID const& entityID);
    static Terran::Core::Shared<ScriptInstance> CreateScriptInstance(Entity entity);
    static void DestroyScriptInstance(Entity entity);

    static void OnStart(Entity entity);
    static void OnUpdate(Entity entity, float deltaTime);

    static void OnPhysicsBeginContact(Entity collider, Entity collidee);
    static void OnPhysicsEndContact(Entity collider, Entity collidee);
    static void OnPhysicsUpdate(Entity entity);

    static void const* CreateEntityInstance(Terran::Core::UUID const& id);
    static int32_t GetEntityIDFieldHandle();

    static void const* CreateComponentInstance(int32_t componentTypeId, Terran::Core::UUID const& entityId);

    static bool LoadAppAssembly();

private:
    static bool LoadCoreAssembly();
    static void InitializeTypeConverters();

    friend class ScriptBindings;
};

}
