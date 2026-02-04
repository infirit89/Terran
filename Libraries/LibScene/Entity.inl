#pragma once
namespace Terran::World {

template<typename Component, typename... Args>
inline Component& Entity::AddComponent(Args&&... parameters)
{
    TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

    TR_ASSERT(!HasComponent<Component>(), "Entity already has component");

    Component& component = m_Scene->m_Registry.emplace<Component>(m_Handle, std::forward<Args>(parameters)...);
    return component;
}

template<typename Component, typename... Args>
inline Component& Entity::AddOrReplaceComponent(Args&&... parameters)
{
    TR_ASSERT(m_Handle != entt::null, "Ivalid entity");
    Component& component = m_Scene->m_Registry.emplace_or_replace<Component>(m_Handle, std::forward<Args>(parameters)...);
    return component;
}

template<typename Component>
inline Component& Entity::GetComponent() const
{
    TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

    TR_ASSERT(HasComponent<Component>(), "Entity doesn't have the component");
    return m_Scene->m_Registry.get<Component>(m_Handle);
}

template<typename Component>
inline void Entity::RemoveComponent()
{
    TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

    TR_ASSERT(HasComponent<Component>(), "Entity doesn't have component");

    m_Scene->m_Registry.remove<Component>(m_Handle);
}

template<typename Component>
inline Component& Entity::TryGetComponent() const
{
    TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

    return m_Scene->m_Registry.try_get<Component>(m_Handle);
}

template<typename Component>
inline bool Entity::HasComponent() const
{
    TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

    return m_Scene->m_Registry.all_of<Component>(m_Handle);
}

template<typename Func>
inline void Entity::Visit(Entity entity, Func func) const
{
    m_Scene->m_Registry.visit(entity, std::forward<Func>(func));
}

}
