#pragma once

#include "Components.h"
#include "Scene.h"

#include "LibCore/Assert.h"
#include "LibCore/UUID.h"

#pragma warning(push)
#pragma warning(disable : 4834)

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace TerranEngine {

class Entity final {
public:
    Entity() = default;

    Entity(entt::entity const& handle, Scene* scene)
        : m_Handle(handle)
        , m_Scene(scene)
    {
    }

    ~Entity() = default;

    template<typename Component, typename... Args>
    Component& AddComponent(Args&&... parameters)
    {
        TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

        TR_ASSERT(!HasComponent<Component>(), "Entity already has component");

        Component& component = m_Scene->m_Registry.emplace<Component>(m_Handle, std::forward<Args>(parameters)...);
        return component;
    }

    template<typename Component, typename... Args>
    Component& AddOrReplaceComponent(Args&&... parameters)
    {
        TR_ASSERT(m_Handle != entt::null, "Ivalid entity");
        Component& component = m_Scene->m_Registry.emplace_or_replace<Component>(m_Handle, std::forward<Args>(parameters)...);
        return component;
    }

    template<typename Component>
    Component& GetComponent() const
    {
        TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

        TR_ASSERT(HasComponent<Component>(), "Entity doesn't have the component");
        return m_Scene->m_Registry.get<Component>(m_Handle);
    }

    template<typename Component>
    void RemoveComponent()
    {
        TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

        TR_ASSERT(HasComponent<Component>(), "Entity doesn't have component");

        m_Scene->m_Registry.remove<Component>(m_Handle);
    }

    template<typename Component>
    Component& TryGetComponent() const
    {
        TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

        return m_Scene->m_Registry.try_get<Component>(m_Handle);
    }

    template<typename Component>
    bool HasComponent() const
    {
        TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

        return m_Scene->m_Registry.all_of<Component>(m_Handle);
    }

    // visit all the components of an entity
    // the signiture of Func should be void(const entt::type_info)
    template<typename Func>
    void Visit(Entity entity, Func func) const
    {
        m_Scene->m_Registry.visit(entity, std::forward<Func>(func));
    }

    // base stuffs
    Terran::Core::UUID const& GetID() const { return GetComponent<TagComponent>().ID; }
    TransformComponent& GetTransform() const { return GetComponent<TransformComponent>(); }
    bool Valid() const { return m_Scene->m_Registry.valid(m_Handle); }
    std::string const& GetName() const { return GetComponent<TagComponent>().Name; }

    // operators
    operator entt::entity() const { return m_Handle; }
    bool operator!=(Entity const& other) const { return !(*this == other); }
    operator uint32_t() const { return static_cast<uint32_t>(m_Handle); }
    operator bool() const { return m_Handle != entt::null; }
    bool operator==(Entity const& other) const { return m_Handle == other.m_Handle && m_Scene == other.m_Scene; }

    // relationship component stuffs
    std::vector<Terran::Core::UUID>& GetChildren() const { return GetComponent<RelationshipComponent>().Children; }
    size_t GetChildCount() const { return HasComponent<RelationshipComponent>() ? GetComponent<RelationshipComponent>().Children.size() : 0; }
    Terran::Core::UUID GetParentID() const { return HasComponent<RelationshipComponent>() ? GetComponent<RelationshipComponent>().Parent : Terran::Core::UUID::invalid(); }
    bool HasParent() const { return HasComponent<RelationshipComponent>() ? m_Scene->FindEntityWithUUID(GetComponent<RelationshipComponent>().Parent) : false; }

    Terran::Core::UUID const& GetSceneId() const { return m_Scene->GetHandle(); }

    Entity GetChild(uint32_t index) const
    {
        if (!HasComponent<RelationshipComponent>())
            return {};

        return m_Scene->FindEntityWithUUID(GetChildren()[index]);
    }

    void SetParentID(Terran::Core::UUID const& id)
    {
        if (!HasComponent<RelationshipComponent>())
            return;

        auto& relComp = GetComponent<RelationshipComponent>();
        relComp.Parent = id;
    }

    Entity GetParent() const
    {
        if (!HasComponent<RelationshipComponent>())
            return {};

        return m_Scene->FindEntityWithUUID(GetParentID());
    }

    bool IsChildOf(Entity entity) const
    {
        if (!HasComponent<RelationshipComponent>())
            return false;

        if (!entity.HasComponent<RelationshipComponent>())
            return false;

        return GetParentID() == entity.GetID();
    }

    void SetParent(Entity parent, bool forceTransformUpdate = false)
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

    void Unparent()
    {
        if (!HasComponent<RelationshipComponent>())
            return;

        Terran::Core::UUID parentID = GetComponent<RelationshipComponent>().Parent;
        Entity parent = m_Scene->FindEntityWithUUID(parentID);

        if (!parent)
            return;

        m_Scene->ConvertToWorldSpace(*this);

        auto const& it = std::ranges::find(parent.GetChildren(), GetID());

        if (it != parent.GetChildren().end())
            parent.GetChildren().erase(it);

        SetParentID(Terran::Core::UUID({ 0 }));

        // TODO: if the relationship component is no longer necessary than remove it
    }

    /*void Unparent(Entity parent, Entity child, bool removeRelationship)
    {
            if (!parent.HasComponent<RelationshipComponent>())
                    return;

            if (!child.HasComponent<RelationshipComponent>())
                    return;

            const auto& it = std::find(parent.GetChildren().begin(), parent.GetChildren().end(), child.GetID());

            if (it != parent.GetChildren().end())
                    parent.GetChildren().erase(it);

            if (removeRelationship)
                    child.RemoveComponent<RelationshipComponent>();
            else
            {
                    RelationshipComponent& rc = child.GetComponent<RelationshipComponent>();
                    rc.ParentID = UUID({ 0 });
            }
    }*/

    void RemoveChild(Entity child, bool removeRelationship)
    {
        child.Unparent();
    }

    void Reparent(Entity previousParent, Entity newParent)
    {
        Unparent();
        SetParent(newParent);
    }

private:
    entt::entity m_Handle { entt::null };
    Scene* m_Scene = nullptr;
};

}
#pragma warning(pop)
