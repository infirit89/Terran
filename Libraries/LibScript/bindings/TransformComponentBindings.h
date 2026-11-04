#pragma once

#include "BindingBase.h"

#include <LibCore/Base.h>
#include <LibCore/UUID.h>

#include <Coral/Assembly.hpp>
#include <LibScene/Components.h>
#include <LibScene/SceneManager.h>
#include <glm/glm.hpp>

namespace Terran::Script {

class TransformComponentBindings final {
public:
    static void bind(Coral::ManagedAssembly& assembly, Core::RawPtr<World::SceneManager> scene_manager)
    {
        s_scene_manager = scene_manager;

        TR_BIND_INTERNAL_FUNC(Transform_GetPositionICall);
        TR_BIND_INTERNAL_FUNC(Transform_SetPositionICall);

        TR_BIND_INTERNAL_FUNC(Transform_GetRotationICall);
        TR_BIND_INTERNAL_FUNC(Transform_SetRotationICall);

        TR_BIND_INTERNAL_FUNC(Transform_GetScaleICall);
        TR_BIND_INTERNAL_FUNC(Transform_SetScaleICall);

        TR_BIND_INTERNAL_FUNC(Transform_IsDirtyICall);

        TR_BIND_INTERNAL_FUNC(Transform_GetForwardICall);
        TR_BIND_INTERNAL_FUNC(Transform_GetUpICall);
        TR_BIND_INTERNAL_FUNC(Transform_GetRightICall);
    }

private:
    // NOTE: just learned about member pointers pretty cool concept
    template<typename TValue>
    static void update_transform_property(Core::UUID const& id, TValue World::TransformComponent::* member, TValue value)
    {
        World::Entity entity = s_scene_manager->current_scene()->find_entity(id);

        if (!entity)
            return;

        auto transform_component = entity.get_component<World::TransformComponent>();

        transform_component.*member = std::move(value);
        transform_component.IsDirty = true;
    }

    template<typename TValue>
    static TValue get_transform_property(Core::UUID const& id, TValue World::TransformComponent::* member, TValue default_value)
    {
        World::Entity entity = s_scene_manager->current_scene()->find_entity(id);

        if (!entity)
            return default_value;

        return entity.get_component<World::TransformComponent>().*member;
    }

    static glm::vec3 Transform_GetPositionICall(Core::UUID const& id)
    {
        return get_transform_property(id, &World::TransformComponent::Position, { 0.0f, 0.0f, 0.0f });
    }

    static void Transform_SetPositionICall(Core::UUID const& id, glm::vec3 const& inPosition)
    {
        update_transform_property(id, &World::TransformComponent::Position, inPosition);
    }

    static glm::vec3 Transform_GetRotationICall(Core::UUID const& id)
    {
        return get_transform_property(id, &World::TransformComponent::Rotation, { 0.0f, 0.0f, 0.0f });
    }
    static void Transform_SetRotationICall(Core::UUID const& id, glm::vec3 const& inRotation)
    {
        update_transform_property(id, &World::TransformComponent::Rotation, inRotation);
    }

    static glm::vec3 Transform_GetScaleICall(Core::UUID const& id)
    {
        return get_transform_property(id, &World::TransformComponent::Scale, { 0.0f, 0.0f, 0.0f });
    }

    static void Transform_SetScaleICall(Core::UUID const& id, glm::vec3 const& inScale)
    {
        update_transform_property(id, &World::TransformComponent::Scale, inScale);
    }

    static bool Transform_IsDirtyICall(Core::UUID const& id)
    {
        return get_transform_property(id, &World::TransformComponent::IsDirty, false);
    }

    static glm::vec3 Transform_GetForwardICall(Core::UUID const& id)
    {
        return get_transform_property(id, &World::TransformComponent::Forward, { 0.0f, 0.0f, 0.0f });
    }
    static glm::vec3 Transform_GetUpICall(Core::UUID const& id)
    {
        return get_transform_property(id, &World::TransformComponent::Up, { 0.0f, 0.0f, 0.0f });
    }
    static glm::vec3 Transform_GetRightICall(Core::UUID const& id)
    {
        return get_transform_property(id, &World::TransformComponent::Right, { 0.0f, 0.0f, 0.0f });
    }

    inline static Core::RawPtr<World::SceneManager> s_scene_manager;
};

}
