#pragma once

#include "BindingBase.h"

#include <LibCore/Base.h>
#include <LibCore/Log.h>

#include <LibScene/Entity.h>
#include <LibScene/SceneManager.h>

#include <LibScript/ScriptComponent.h>
#include <LibScript/ScriptEngine.h>
#include <LibScript/ScriptTypes.h>

#include <Coral/Assembly.hpp>
#include <cstdint>
#include <functional>
#include <spdlog/common.h>
#include <string>
#include <vector>

namespace Terran::Script {

// NOTE: maybe pass the ScriptEngine instance to C# as an IntPtr and then call internal functions with that instance
class EntityBinding final {
public:
    EntityBinding(Coral::ManagedAssembly& assembly,
        Core::RawPtr<World::SceneManager> scene_manager,
        Core::RawPtr<ScriptEngine> script_engine)
        : m_scene_manager(scene_manager)
        , m_script_engine(script_engine)

    {
        s_instance = this;
    }

private:
    using has_component_delegate = std::function<bool(World::Entity)>;
    using add_component_delegate = std::function<void(World::Entity)>;
    using remove_component_delegate = std::function<void(World::Entity)>;

    static bool Entity_HasComponentICall(Core::UUID const& id, int32_t typeId)
    {
        World::Entity entity = s_instance->m_scene_manager->current_scene()->find_entity(id);
        if (!entity)
            return false;

        auto& has_component_delegates = s_instance->m_has_component_type_delegate_map;
        if (has_component_delegates.contains(typeId))
            return has_component_delegates.at(typeId)(entity);

        Coral::Type* type = Coral::TypeCache::Get().GetTypeByID(typeId);
        if (!type)
            return false;

        if (s_instance->m_script_engine->is_type_scriptable(*type) && entity.has_component<ScriptComponent>()) {
            Coral::ScopedString typeName = type->GetFullName();
            return entity.get_component<ScriptComponent>().ModuleName == typeName;
        }

        return false;
    }

    static void Entity_AddComponentICall(Core::UUID const& id, int32_t typeId)
    {
        World::Entity entity = s_instance->m_scene_manager->current_scene()->find_entity(id);

        if (!entity)
            return;

        auto& add_component_delegates = s_instance->m_add_component_type_delegate_map;
        if (add_component_delegates.contains(typeId)) {
            add_component_delegates.at(typeId)(entity);
            return;
        }

        Coral::Type* type = Coral::TypeCache::Get().GetTypeByID(typeId);
        if (s_instance->m_script_engine->is_type_scriptable(*type)) {
            Coral::ScopedString typeName = type->GetFullName();
            entity.add_component<ScriptComponent>((std::string)typeName);
        }
    }

    static void Entity_RemoveComponentICall(Core::UUID const& id, int32_t typeId)
    {
        World::Entity entity = s_instance->m_scene_manager->current_scene()->find_entity(id);

        if (!entity)
            return;

        auto& remove_component_delegates = s_instance->m_remove_component_type_delegate_map;
        if (remove_component_delegates.contains(typeId))
            remove_component_delegates.at(typeId)(entity);

        // TODO: handle removing script component
    }

    static void* Entity_GetScriptableComponentICall(Core::UUID const& id)
    {
        World::Entity entity = s_instance->m_scene_manager->current_scene()->find_entity(id);
        if (!entity)
            return nullptr;

        Core::Shared<ScriptInstance> scriptInstance = s_instance->m_script_engine->script_instance(entity);
        return (void*)scriptInstance->handle();
    }

    static bool Entity_FindEntityWithNameICall(Coral::String entityName, Core::UUID& id);

    static void Entity_DestroyEntityICall(Core::UUID const& id);

    static int Entity_GetChildrenCountICall(Core::UUID const& id);
    static Core::UUID Entity_GetChildICall(Core::UUID const& id, int index);
    static Core::UUID Entity_GetParentICall(Core::UUID const& id);

    Core::RawPtr<World::SceneManager> m_scene_manager;
    Core::RawPtr<ScriptEngine> m_script_engine;
    std::unordered_map<int32_t, has_component_delegate> m_has_component_type_delegate_map;
    std::unordered_map<int32_t, add_component_delegate> m_add_component_type_delegate_map;
    std::unordered_map<int32_t, remove_component_delegate> m_remove_component_type_delegate_map;
    inline static EntityBinding* s_instance = nullptr;
};

}
