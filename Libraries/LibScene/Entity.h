#pragma once

#include "Components.h"

#include <LibCore/Assert.h>
#include <LibCore/UUID.h>
#include <entt.hpp>

#pragma warning(push)
#pragma warning(disable : 4834)

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace Terran::World {

class Scene;

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
    Component& AddComponent(Args&&... parameters);

    template<typename Component, typename... Args>
    Component& AddOrReplaceComponent(Args&&... parameters);

    template<typename Component>
    Component& GetComponent() const;

    template<typename Component>
    void RemoveComponent();

    template<typename Component>
    Component& TryGetComponent() const;

    template<typename Component>
    bool HasComponent() const;

    // visit all the components of an entity
    // the signiture of Func should be void(const entt::type_info)
    template<typename Func>
    void Visit(Entity entity, Func func) const;

    // base stuffs
    Terran::Core::UUID const& GetID() const { return GetComponent<TagComponent>().ID; }
    TransformComponent& GetTransform() const { return GetComponent<TransformComponent>(); }
    bool Valid() const;
    std::string const& GetName() const { return GetComponent<TagComponent>().Name; }

    // operators
    operator entt::entity() const { return m_Handle; }
    bool operator!=(Entity const& other) const { return !(*this == other); }
    operator uint32_t() const { return static_cast<uint32_t>(m_Handle); }
    operator bool() const { return m_Handle != entt::null; }
    bool operator==(Entity const& other) const;

    // relationship component stuffs
    std::vector<Terran::Core::UUID>& GetChildren() const { return GetComponent<RelationshipComponent>().Children; }
    size_t GetChildCount() const { return HasComponent<RelationshipComponent>() ? GetComponent<RelationshipComponent>().Children.size() : 0; }
    Terran::Core::UUID GetParentID() const { return HasComponent<RelationshipComponent>() ? GetComponent<RelationshipComponent>().Parent : Terran::Core::UUID::invalid(); }
    bool HasParent() const;

    Terran::Core::UUID const& GetSceneId() const;

    Entity GetChild(uint32_t index) const;

    void SetParentID(Terran::Core::UUID const& id)
    {
        if (!HasComponent<RelationshipComponent>())
            return;

        auto& relComp = GetComponent<RelationshipComponent>();
        relComp.Parent = id;
    }

    Entity GetParent() const;

    bool IsChildOf(Entity entity) const
    {
        if (!HasComponent<RelationshipComponent>())
            return false;

        if (!entity.HasComponent<RelationshipComponent>())
            return false;

        return GetParentID() == entity.GetID();
    }

    void SetParent(Entity parent, bool forceTransformUpdate = false);

    void Unparent();

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
