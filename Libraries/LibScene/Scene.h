#pragma once

#include <LibCore/Base.h>
#include <LibCore/Event.h>
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
#include <string_view>
#include <unordered_map>

namespace Terran::World {

class Scene final : public Asset::Asset {
public:
    explicit Scene(Core::EventDispatcher& event_dispatcher);
    Scene(Core::EventDispatcher& event_dispatcher, Core::UUID const& handle);
    ~Scene() override;

    TR_DECLARE_ASSET_TYPE(Scene)

    Entity create_entity(std::string const& name = std::string());
    Entity create_entity(std::string const& name, Core::UUID const& uuid);
    Entity create_entity();

    void destrory_entity(Entity entity, bool first);

    void start_runtime();
    void stop_runtime();

    void update(Core::Time time);

    Entity find_entity(Core::UUID const& uuid);
    Entity find_entity(std::string_view name);

    template<typename... Args, typename... Exclude>
    auto entities_with(entt::exclude_t<Exclude...> exclude = {})
    {
        return m_registry.view<Args...>(exclude);
    }

    std::unordered_map<Core::UUID, entt::entity>& GetEntityMap()
    {
        return m_entity_map;
    }

    Entity duplicate_entity(Entity source_entity, Entity parent = {});

    bool playing() const
    {
        return m_is_playing;
    }


    void update_transform_hierarchy();
    void update_entity_transform(Entity entity);

    void convert_to_local_space(Entity entity);
    void convert_to_world_space(Entity entity);

    void sort_entities();

private:

    template<typename TComponent>
    static void copy_component(Entity source_entity, Entity destination_entity, entt::registry& source_registry, entt::registry& destination_registry)
    {
        if (!source_registry.all_of<TComponent>((entt::entity)source_entity)) {
            return;
        }

        destination_registry.emplace_or_replace<TComponent>(
            (entt::entity)destination_entity,
            source_registry.get<TComponent>((entt::entity)source_entity));
    }

    bool m_is_playing = false;

    std::unordered_map<Core::UUID, entt::entity> m_entity_map;
    Core::EventDispatcher& m_event_dispatcher;

    entt::registry m_registry;

    friend class Entity;
    friend class SceneSerializer;
    friend class SceneManager;
};

}
#include "Entity.inl"
