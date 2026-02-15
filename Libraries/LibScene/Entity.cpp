#include "Entity.h"
#include "Scene.h"
#include "Components.h"

#include <LibCore/UUID.h>

#include <cstdint>

namespace Terran::World {

bool Entity::valid() const
{
    return m_scene->m_registry.valid(m_handle);
}
bool Entity::operator==(Entity const& other) const
{
    return m_handle == other.m_handle && m_scene == other.m_scene;
}
bool Entity::has_parent() const
{
    return has_component<RelationshipComponent>() ? m_scene->find_entity(get_component<RelationshipComponent>().Parent) : false;
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
void Entity::set_parent(Entity parent, bool forceTransformUpdate)
{
    if (!has_component<RelationshipComponent>())
        add_component<RelationshipComponent>();

    if (!parent.has_component<RelationshipComponent>())
        parent.add_component<RelationshipComponent>();

    if (is_child_of(parent))
        return;
    if (parent.is_child_of(*this))
        return;

    if (has_parent())
        unparent();

    auto& relComp = get_component<RelationshipComponent>();
    relComp.Parent = parent.id();
    parent.children().emplace_back(id());

    m_scene->convert_to_local_space(*this);
}
void Entity::unparent()
{
    if (!has_component<RelationshipComponent>())
        return;

    Core::UUID parentID = get_component<RelationshipComponent>().Parent;
    Entity parent = m_scene->find_entity(parentID);

    if (!parent)
        return;

    m_scene->convert_to_world_space(*this);

    auto const& it = std::ranges::find(parent.children(), id());

    if (it != parent.children().end())
        parent.children().erase(it);

    set_parent_id(Core::UUID({ 0 }));

    // TODO: if the relationship component is no longer necessary than remove it
}

}
