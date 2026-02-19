#include "Entity.h"
#include "Components.h"
#include "Scene.h"

#include <LibCore/Result.h>
#include <LibCore/UUID.h>

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
    return m_scene->handle();
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

    if (is_child_of(parent))
        return { EntityErrors::AlreadyAChildOfThisParent };
    if (parent.is_child_of(*this))
        return { EntityErrors::TargetParentIsAlreadyAChildOfThisEntity };

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
    RelationshipComponent* relationship_component = try_get_component<RelationshipComponent>();
    if (!relationship_component) {
        return { EntityErrors::DoesntHaveRelationshipComponent };
    }
    Core::UUID parent_id = relationship_component->Parent;
    Entity parent = m_scene->find_entity(parent_id);

    if (!parent)
        return { EntityErrors::ParentNotFound };

    RelationshipComponent* parent_relationship_component = parent.try_get_component<RelationshipComponent>();

    if (!parent_relationship_component) {
        return { EntityErrors::DoesntHaveRelationshipComponent };
    }

    auto& children = parent_relationship_component->Children;
    auto const& iterator = std::ranges::find(children, id());
    if (iterator == parent.children().end()) {
        return { EntityErrors::ParentDoesntContainChild };
    }

    children.erase(iterator);
    set_parent_id(Core::UUID({ 0 }));

    m_scene->convert_to_world_space(*this);
    return {};
}

}
