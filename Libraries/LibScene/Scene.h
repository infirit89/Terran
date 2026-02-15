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
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace Terran::World {

class Scene final : public Asset::Asset {
public:
    Scene();
    Scene(Terran::Core::UUID const& handle);
    ~Scene() override;

    TR_DECLARE_ASSET_TYPE(Scene)

    Entity CreateEntity(std::string const& name = std::string());
    Entity CreateEntityWithUUID(std::string const& name, Terran::Core::UUID const& uuid);
    Entity CreateEmptyEntity();

    void DestroyEntity(Entity entity, bool first);

    void StartRuntime();
    void StopRuntime();

    void Update(Terran::Core::Time time);

    Entity FindEntityWithUUID(Terran::Core::UUID uuid);
    Entity FindEntityWithName(std::string const& name);

    template<typename... Args, typename... Exclude>
    auto GetEntitiesWith(entt::exclude_t<Exclude...> exclude = {}) { return m_Registry.view<Args...>(exclude); }

    std::unordered_map<Terran::Core::UUID, entt::entity>& GetEntityMap() { return m_EntityMap; }


    Entity DuplicateEntity(Entity srcEntity, Entity parent);
    Entity DuplicateEntity(Entity srcEntity);

    static Terran::Core::Shared<Scene> CopyScene(Terran::Core::Shared<Scene> const& srcScene);

    bool IsPlaying() const { return m_IsPlaying; }

    Scene* GetRaw() { return this; }

    void UpdateTransformHierarchy();
    void UpdateEntityTransform(Entity entity);

    void ConvertToLocalSpace(Entity entity);
    void ConvertToWorldSpace(Entity entity);

    void SortEntities();

private:
    bool m_IsPlaying = false;

    std::unordered_map<Terran::Core::UUID, entt::entity> m_EntityMap;

    entt::registry m_Registry;

    friend class Entity;
    friend class SceneSerializer;
};

}
#include "Entity.inl"
