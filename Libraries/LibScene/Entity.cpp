#include "Scene.h"

#include "LibCore/UUID.h"

namespace Terran::World {

bool Entity::Valid() const { return m_Scene->m_Registry.valid(m_Handle); }
bool Entity::operator==(Entity const& other) const { return m_Handle == other.m_Handle && m_Scene == other.m_Scene; }
bool Entity::HasParent() const { return HasComponent<RelationshipComponent>() ? m_Scene->FindEntityWithUUID(GetComponent<RelationshipComponent>().Parent) : false; }
Core::UUID const& Entity::GetSceneId() const { return m_Scene->handle(); }

Entity Entity::GetChild(uint32_t index) const
{
    if (!HasComponent<RelationshipComponent>())
        return {};

    return m_Scene->FindEntityWithUUID(GetChildren()[index]);
}

Entity Entity::GetParent() const
{
    if (!HasComponent<RelationshipComponent>())
        return {};

    return m_Scene->FindEntityWithUUID(GetParentID());
}
void Entity::SetParent(Entity parent, bool forceTransformUpdate)
{
    if (!HasComponent<RelationshipComponent>())
        AddComponent<RelationshipComponent>();

    if (!parent.HasComponent<RelationshipComponent>())
        parent.AddComponent<RelationshipComponent>();

    if (IsChildOf(parent))
        return;
    if (parent.IsChildOf(*this))
        return;

    if (HasParent())
        Unparent();

    auto& relComp = GetComponent<RelationshipComponent>();
    relComp.Parent = parent.GetID();
    parent.GetChildren().emplace_back(GetID());

    m_Scene->ConvertToLocalSpace(*this);
}
void Entity::Unparent()
{
    if (!HasComponent<RelationshipComponent>())
        return;

    Core::UUID parentID = GetComponent<RelationshipComponent>().Parent;
    Entity parent = m_Scene->FindEntityWithUUID(parentID);

    if (!parent)
        return;

    m_Scene->ConvertToWorldSpace(*this);

    auto const& it = std::ranges::find(parent.GetChildren(), GetID());

    if (it != parent.GetChildren().end())
        parent.GetChildren().erase(it);

    SetParentID(Core::UUID({ 0 }));

    // TODO: if the relationship component is no longer necessary than remove it
}

}
