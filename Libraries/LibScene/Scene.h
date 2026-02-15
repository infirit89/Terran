#pragma once

#include <LibCore/Base.h>
#include <LibCore/Time.h>
#include <LibCore/UUID.h>

#include <LibAsset/Asset.h>

#include "Entity.h"

#pragma warning(push)
#pragma warning(disable : 26439)
#include <entt.hpp>
#pragma warning(pop)
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>
#include <string_view>

namespace Terran::World {

class Scene final : public Asset::Asset {
public:
    Scene();
    explicit Scene(Terran::Core::UUID const& handle);
    ~Scene() override;

    TR_DECLARE_ASSET_TYPE(Scene)

    Entity create_entity(std::string const& name = std::string());
    Entity create_entity(std::string const& name, Terran::Core::UUID const& uuid);
    Entity create_entity();

    void destrory_entity(Entity entity, bool first);

    void start_runtime();
    void stop_runtime();

    void update(Terran::Core::Time time);

    Entity find_entity(Terran::Core::UUID uuid);
    Entity find_entity(std::string_view name);

    template<typename... Args, typename... Exclude>
    auto entities_with(entt::exclude_t<Exclude...> exclude = {}) { return m_registry.view<Args...>(exclude); }

    std::unordered_map<Terran::Core::UUID, entt::entity>& GetEntityMap() { return m_entity_map; }


    Entity duplicate_entity(Entity srcEntity, Entity parent);
    Entity duplicate_entity(Entity srcEntity);

    static Terran::Core::Shared<Scene> copy_scene(Terran::Core::Shared<Scene> const& srcScene);

    bool playing() const { return m_is_playing; }

    Scene* raw() { return this; }

    void update_transform_hierarchy();
    void update_entity_transform(Entity entity);

    void convert_to_local_space(Entity entity);
    void convert_to_world_space(Entity entity);

    void sort_entities();

private:
    bool m_is_playing = false;

    std::unordered_map<Terran::Core::UUID, entt::entity> m_entity_map;

    entt::registry m_registry;

    friend class Entity;
    friend class SceneSerializer;
};

}
#include "Entity.inl"
