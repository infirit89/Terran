#pragma once

#include "Components.h"

#include <LibCore/Assert.h>
#include <LibCore/Result.h>
#include <LibCore/UUID.h>

#include <entt.hpp>

#pragma warning(push)
#pragma warning(disable : 4834)

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace Terran::World {

class Scene;

enum class EntityErrors : uint8_t {
    AlreadyAChildOfThisParent,
    DoesntHaveRelationshipComponent,
    TargetParentIsAlreadyAChildOfThisEntity,
    ParentNotFound,
    ParentDoesntContainChild
};

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
    auto try_get_component() const;

    template<typename Component>
    bool has_component() const;

    // visit all the components of an entity
    // the signiture of Func should be void(const entt::type_info)
    template<typename Func>
    void visit(Entity entity, Func func) const;

    // base stuffs
    Core::UUID const& id() const
    {
        return has_component<TagComponent>()
            ? get_component<TagComponent>().Id
            : Core::UUID::invalid();
    }

    TransformComponent& transform() const
    {
        return get_component<TransformComponent>();
    }

    bool valid() const;

    std::string const& name() const
    {
        return get_component<TagComponent>().Name;
    }

    // operators
    explicit operator entt::entity() const
    {
        return m_handle;
    }

    explicit operator uint32_t() const
    {
        return static_cast<uint32_t>(m_handle);
    }

    explicit operator bool() const
    {
        return m_handle != entt::null;
    }

    bool operator==(Entity const& other) const = default;

    // relationship component stuffs
    std::span<Core::UUID> children() const
    {
        return has_component<RelationshipComponent>()
            ? get_component<RelationshipComponent>().Children
            : std::span<Core::UUID>();
    }

    Terran::Core::UUID parent_id() const
    {
        return has_component<RelationshipComponent>() ? get_component<RelationshipComponent>().Parent : Terran::Core::UUID::invalid();
    }

    bool has_parent() const;

    Terran::Core::UUID const& scene_id() const;

    Entity child_at(uint32_t index) const;

    void set_parent_id(Terran::Core::UUID const& id) const
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

    Core::Result<void, EntityErrors> set_parent(Entity parent);

    Core::Result<void, EntityErrors> unparent();

    void remove_child(Entity child) const
    {
        child.unparent();
    }

    void reparent(Entity newParent)
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
