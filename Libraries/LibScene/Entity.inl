#pragma once
namespace Terran::World {

template<typename Component, typename... Args>
inline Component& Entity::add_component(Args&&... parameters)
{
    TR_ASSERT(m_handle != entt::null, "Ivalid entity");

    TR_ASSERT(!has_component<Component>(), "Entity already has component");

    Component& component = m_scene->m_registry.emplace<Component>(m_handle, std::forward<Args>(parameters)...);
    return component;
}

template<typename Component, typename... Args>
inline Component& Entity::add_or_replace_component(Args&&... parameters)
{
    TR_ASSERT(m_handle != entt::null, "Ivalid entity");
    Component& component = m_scene->m_registry.emplace_or_replace<Component>(m_handle, std::forward<Args>(parameters)...);
    return component;
}

template<typename Component>
inline Component& Entity::get_component() const
{
    TR_ASSERT(m_handle != entt::null, "Ivalid entity");

    TR_ASSERT(has_component<Component>(), "Entity doesn't have the component");
    return m_scene->m_registry.get<Component>(m_handle);
}

template<typename Component>
inline void Entity::remove_component()
{
    TR_ASSERT(m_handle != entt::null, "Ivalid entity");

    TR_ASSERT(has_component<Component>(), "Entity doesn't have component");

    m_scene->m_registry.remove<Component>(m_handle);
}

template<typename Component>
inline auto Entity::try_get_component() const
{
    TR_ASSERT(m_handle != entt::null, "Ivalid entity");

    return m_scene->m_registry.try_get<Component>(m_handle);
}

template<typename Component>
inline bool Entity::has_component() const
{
    TR_ASSERT(m_handle != entt::null, "Ivalid entity");

    return m_scene->m_registry.all_of<Component>(m_handle);
}

template<typename Func>
inline void Entity::visit(Entity entity, Func func) const
{
    m_scene->m_registry.visit(entity, std::forward<Func>(func));
}

}
