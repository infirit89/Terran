#include "Entity.h"
#include "Components.h"
#include "Scene.h"

#include <LibCore/Result.h>
#include <LibCore/UUID.h>
#include <LibCore/Log.h>

#include <LibScene/SceneTypes.h>
#include <algorithm>
#include <cstdint>

namespace Terran::World {

bool Entity::valid() const
{
    return m_scene->m_registry.valid(m_handle);
}

bool Entity::has_parent() const
{
    return has_component<RelationshipComponent>()
        ? (bool)m_scene->find_entity(get_component<RelationshipComponent>().Parent)
        : false;
}
Core::UUID const& Entity::scene_id() const
{
    return m_scene->id();
}

Entity Entity::child_at(uint32_t index) const
{
    if (!has_component<RelationshipComponent>())
        return {};

    return m_scene->find_entity(children()[index]);
}

Entity Entity::parent() const
{
    if (!has_component<RelationshipComponent>())
        return {};

    return m_scene->find_entity(parent_id());
}

Core::Result<void, EntityErrors> Entity::set_parent(Entity parent)
{
    if (!has_component<RelationshipComponent>())
        add_component<RelationshipComponent>();

    if (!parent.has_component<RelationshipComponent>())
        parent.add_component<RelationshipComponent>();

    if (is_child_of(parent)) {
        TR_WARN(TR_LOG_SCENE, "Trying to parent an entity with Id: {} which is already a child of {}", id(), parent.id());
        return { EntityErrors::AlreadyAChildOfThisParent };
    }
    if (parent.is_child_of(*this)) {
        // NOTE: this is a terrible error message, couldn't think of a better one right now though
        TR_WARN(TR_LOG_SCENE, "Trying to parent to an entity {} which is a child of the current entity with Id: {}", parent.id(), id());
        return { EntityErrors::TargetParentIsAlreadyAChildOfThisEntity };
    }

    if (has_parent())
        unparent();

    auto& relationship_component = get_component<RelationshipComponent>();
    relationship_component.Parent = parent.id();

    auto& parent_relationship_component = parent.get_component<RelationshipComponent>();
    parent_relationship_component.Children.emplace_back(id());

    m_scene->convert_to_local_space(*this);
    return {};
}
Core::Result<void, EntityErrors> Entity::unparent()
{
    RelationshipComponent const* relationship_component = try_get_component<RelationshipComponent>();

    if (!relationship_component) {
        TR_WARN(TR_LOG_SCENE, "Trying to unparent an enity with Id: {} which doesn't have a relationship component", id());
        return { EntityErrors::DoesntHaveRelationshipComponent };
    }

    Core::UUID parent_id = relationship_component->Parent;
    Entity parent = m_scene->find_entity(parent_id);

    if (!parent) {
        TR_WARN(TR_LOG_SCENE, "Failed to find parent {} while trying to unparent", parent_id);
        return { EntityErrors::ParentNotFound };
    }

    RelationshipComponent* parent_relationship_component = parent.try_get_component<RelationshipComponent>();

    if (!parent_relationship_component) {
        TR_WARN(TR_LOG_SCENE, "Parent {} doesn't have relationship component! Failed to unparent", parent_id);
        return { EntityErrors::DoesntHaveRelationshipComponent };
    }

    auto& children = parent_relationship_component->Children;
    auto const& iterator = std::ranges::find(children, id());
    if (iterator == children.end()) {
        TR_WARN(TR_LOG_SCENE, "Parent {} doesn't contain entity: {}! Failed to unparent", parent_id, id());
        return { EntityErrors::ParentDoesntContainChild };
    }

    children.erase(iterator);
    set_parent_id(Core::UUID({ 0 }));

    m_scene->convert_to_world_space(*this);
    return {};
}

}
