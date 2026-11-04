#pragma once

#include "BindingBase.h"

#include <LibCore/Base.h>
#include <LibCore/Log.h>

#include <LibCore/UUID.h>
#include <LibScene/Entity.h>
#include <LibScene/SceneManager.h>

#include <LibScript/ScriptComponent.h>
#include <LibScript/ScriptInstance.h>
#include <LibScript/ScriptTypes.h>
#include <LibScript/ScriptEngine.h>

#include <Coral/Assembly.hpp>
#include <Coral/TypeCache.hpp>
#include <cstdint>
#include <spdlog/common.h>

namespace Terran::Script {

// NOTE: maybe pass the ScriptEngine instance to C# as an IntPtr and then call internal functions with that instance
class EntityBinding final {
public:
    static void bind(Coral::ManagedAssembly& assembly,
        Core::RawPtr<World::SceneManager> scene_manager,
        Core::RawPtr<ScriptEngine> script_engine)

    {
        s_scene_manager = scene_manager;
        s_script_engine = script_engine;
        TR_BIND_INTERNAL_FUNC(Entity_AddComponentICall);
        TR_BIND_INTERNAL_FUNC(Entity_RemoveComponentICall);
        TR_BIND_INTERNAL_FUNC(Entity_HasComponentICall);
        TR_BIND_INTERNAL_FUNC(Entity_GetScriptableComponentICall);
        TR_BIND_INTERNAL_FUNC(Entity_FindEntityWithNameICall);
        TR_BIND_INTERNAL_FUNC(Entity_DestroyEntityICall);
        TR_BIND_INTERNAL_FUNC(Entity_GetChildrenCountICall);
        TR_BIND_INTERNAL_FUNC(Entity_GetChildICall);
        TR_BIND_INTERNAL_FUNC(Entity_GetParentICall);
    }

private:
    static bool Entity_HasComponentICall(Core::UUID const& id, int32_t typeId)
    {
        World::Entity entity = s_scene_manager->current_scene()->find_entity(id);
        if (!entity)
            return false;

        auto& has_component_delegates = s_script_engine->m_has_component_type_delegate_map;
        if (has_component_delegates.contains(typeId))
            return has_component_delegates.at(typeId)(entity);

        Coral::Type* type = Coral::TypeCache::Get().GetTypeByID(typeId);
        if (!type)
            return false;

        if (s_script_engine->is_type_scriptable(*type) && entity.has_component<ScriptComponent>()) {
            Coral::ScopedString typeName = type->GetFullName();
            return entity.get_component<ScriptComponent>().ModuleName == typeName;
        }

        return false;
    }

    static void Entity_AddComponentICall(Core::UUID const& id, int32_t typeId)
    {
        World::Entity entity = s_scene_manager->current_scene()->find_entity(id);

        if (!entity)
            return;

        auto& add_component_delegates = s_script_engine->m_add_component_type_delegate_map;
        if (add_component_delegates.contains(typeId)) {
            add_component_delegates.at(typeId)(entity);
            return;
        }

        Coral::Type* type = Coral::TypeCache::Get().GetTypeByID(typeId);
        if (s_script_engine->is_type_scriptable(*type)) {
            Coral::ScopedString typeName = type->GetFullName();
            entity.add_component<ScriptComponent>((std::string)typeName);
        }
    }

    static void Entity_RemoveComponentICall(Core::UUID const& id, int32_t typeId)
    {
        World::Entity entity = s_scene_manager->current_scene()->find_entity(id);

        if (!entity)
            return;

        auto& remove_component_delegates = s_script_engine->m_remove_component_type_delegate_map;
        if (remove_component_delegates.contains(typeId))
            remove_component_delegates.at(typeId)(entity);

        // TODO: handle removing script component
    }

    static void* Entity_GetScriptableComponentICall(Core::UUID const& id)
    {

        World::Entity entity = s_scene_manager->current_scene()->find_entity(id);
        if (!entity)
            return nullptr;

        Core::Shared<ScriptInstance> scriptInstance = s_script_engine->script_instance(entity);
        return (void*)scriptInstance->handle();
    }

    static bool Entity_FindEntityWithNameICall(Coral::String entityName, Core::UUID& id)
    {
        World::Entity entity = s_scene_manager->current_scene()->find_entity(entityName.Data());

        if (!entity)
            return false;

        id = entity.id();
        return true;
    }

    static void Entity_DestroyEntityICall(Core::UUID const& id)
    {
        World::Entity entity = s_scene_manager->current_scene()->find_entity(id);
        s_scene_manager->current_scene()->destrory_entity(entity);
    }

    static int Entity_GetChildrenCountICall(Core::UUID const& id)
    {
        World::Entity entity = s_scene_manager->current_scene()->find_entity(id);
        if (!entity)
            return 0;

        return static_cast<int>(entity.children().size());
    }
    static Core::UUID Entity_GetChildICall(Core::UUID const& id, int index)
    {
        World::Entity entity = s_scene_manager->current_scene()->find_entity(id);
        if (!entity)
            return Core::UUID::invalid();

        return entity.child_at(index).id();
    }

    static Core::UUID Entity_GetParentICall(Core::UUID const& id)
    {
        World::Entity entity = s_scene_manager->current_scene()->find_entity(id);
        if (!entity)
            return Core::UUID::invalid();

        return entity.parent_id();
    }

    inline static Core::RawPtr<World::SceneManager> s_scene_manager = nullptr;
    inline static Core::RawPtr<ScriptEngine> s_script_engine = nullptr;
};

}
