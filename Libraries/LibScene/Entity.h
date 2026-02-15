#pragma once

#include "Components.h"

#include <LibCore/Assert.h>
#include <LibCore/UUID.h>
#include <entt.hpp>

#pragma warning(push)
#pragma warning(disable : 4834)

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
        : m_handle(handle)
        , m_scene(scene)
    {
    }

    ~Entity() = default;

    template<typename Component, typename... Args>
    Component& add_component(Args&&... parameters);

    template<typename Component, typename... Args>
    Component& add_or_replace_component(Args&&... parameters);

    template<typename Component>
    Component& get_component() const;

    template<typename Component>
    void remove_component();

    template<typename Component>
    Component& try_get_component() const;

    template<typename Component>
    bool has_component() const;

    // visit all the components of an entity
    // the signiture of Func should be void(const entt::type_info)
    template<typename Func>
    void visit(Entity entity, Func func) const;

    // base stuffs
    Terran::Core::UUID const& id() const { return get_component<TagComponent>().ID; }
    TransformComponent& transform() const { return get_component<TransformComponent>(); }
    bool valid() const;
    std::string const& name() const { return get_component<TagComponent>().Name; }

    // operators
    operator entt::entity() const { return m_handle; }
    bool operator!=(Entity const& other) const { return !(*this == other); }
    operator uint32_t() const { return static_cast<uint32_t>(m_handle); }
    operator bool() const { return m_handle != entt::null; }
    bool operator==(Entity const& other) const;

    // relationship component stuffs
    std::vector<Terran::Core::UUID>& children() const { return get_component<RelationshipComponent>().Children; }
    size_t children_count() const { return has_component<RelationshipComponent>() ? get_component<RelationshipComponent>().Children.size() : 0; }
    Terran::Core::UUID parent_id() const { return has_component<RelationshipComponent>() ? get_component<RelationshipComponent>().Parent : Terran::Core::UUID::invalid(); }
    bool has_parent() const;

    Terran::Core::UUID const& scene_id() const;

    Entity child_at(uint32_t index) const;

    void set_parent_id(Terran::Core::UUID const& id)
    {
        if (!has_component<RelationshipComponent>())
            return;

        auto& relComp = get_component<RelationshipComponent>();
        relComp.Parent = id;
    }

    Entity parent() const;

    bool is_child_of(Entity entity) const
    {
        if (!has_component<RelationshipComponent>())
            return false;

        if (!entity.has_component<RelationshipComponent>())
            return false;

        return parent_id() == entity.id();
    }

    void set_parent(Entity parent, bool forceTransformUpdate = false);

    void unparent();

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

    void remove_child(Entity child, bool removeRelationship)
    {
        child.unparent();
    }

    void reparent(Entity previousParent, Entity newParent)
    {
        unparent();
        set_parent(newParent);
    }

private:
    entt::entity m_handle { entt::null };
    Scene* m_scene = nullptr;
};

}
#pragma warning(pop)
