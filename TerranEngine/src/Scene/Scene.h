#pragma once

#include "LibCore/Base.h"
#include "LibCore/Time.h"
#include "LibCore/UUID.h"

#include "Asset/Asset.h"

#include "Graphics/Camera.h"

#pragma warning(push)
#pragma warning(disable : 26439)
#include <entt.hpp>
#pragma warning(pop)

#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace TerranEngine {

class Entity;
class SceneRenderer;

class Scene final : public Asset {
public:
    Scene();
    Scene(Terran::Core::UUID const& handle);
    ~Scene() override;

    ASSET_CLASS_TYPE(Scene)

    Entity CreateEntity(std::string const& name = std::string());
    Entity CreateEntityWithUUID(std::string const& name, Terran::Core::UUID const& uuid);
    Entity CreateEmptyEntity();

    void DestroyEntity(Entity entity, bool first);

    void StartRuntime();
    void StopRuntime();

    void Update(Terran::Core::Time time);
    void UpdateEditor();
    void OnResize(float width, float height);

    void OnRender(Terran::Core::Shared<SceneRenderer> const& sceneRenderer);
    void OnRenderEditor(Terran::Core::Shared<SceneRenderer> const& sceneRenderer, Camera& camera, glm::mat4& cameraView);

    Entity FindEntityWithUUID(Terran::Core::UUID uuid);
    Entity FindEntityWithName(std::string const& name);

    template<typename... Args, typename... Exclude>
    auto GetEntitiesWith(entt::exclude_t<Exclude...> exclude = {}) { return m_Registry.view<Args...>(exclude); }

    std::unordered_map<Terran::Core::UUID, entt::entity>& GetEntityMap() { return m_EntityMap; }

    // template<typename... Components, typename Predicate>
    // std::vector<Entity> Filter(Predicate&& predicate)
    // {
    // 	std::vector<Entity> entities;
    // 	auto view = m_Registry.view<std::add_const_t<Components>...>();
    // 	entities.reserve(view.size());
    //
    // 	for (auto e : view)
    // 	{
    // 		if (predicate(view.template get<std::add_const_t<Components>>(e)...))
    // 			entities.push_back({ e, this });
    // 	}
    //
    // 	return entities;
    // }

    Entity GetPrimaryCamera();

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
    glm::vec2 const& GetViewportPosition() const { return m_ViewportPosition; }
    void SetViewportPosition(glm::vec2 const& viewportPosition) { m_ViewportPosition = viewportPosition; }

    float GetViewportWidth() const { return m_ViewportWidth; }
    float GetViewportHeight() const { return m_ViewportHeight; }

private:
    // scripting components
    void OnScriptComponentConstructed(entt::registry& registry, entt::entity entityHandle);
    void OnScriptComponentDestroyed(entt::registry& registry, entt::entity entityHandle);

    // physics components
    void OnRigidbody2DComponentConstructed(entt::registry& registry, entt::entity entityHandle);
    void OnRigidbody2DComponentDestroyed(entt::registry& registry, entt::entity entityHandle);

    void OnBoxCollider2DComponentConstructed(entt::registry& registry, entt::entity entityHandle);
    void OnBoxCollider2DComponentDestroyed(entt::registry& registry, entt::entity entityHandle);

    void OnCircleCollider2DComponentConstructed(entt::registry& registry, entt::entity entityHandle);
    void OnCircleCollider2DComponentDestroyed(entt::registry& registry, entt::entity entityHandle);

    void OnCapsuleCollider2DComponentConstructed(entt::registry& registry, entt::entity entityHandle);
    void OnCapsuleCollider2DComponentDestroyed(entt::registry& registry, entt::entity entityHandle);

    // text component
    void OnTextComponentConstructed(entt::registry& registry, entt::entity entityHandle);
    void OnTextComponentDestroyed(entt::registry& registry, entt::entity entityHandle);

private:
    bool m_IsPlaying = false;

    std::unordered_map<Terran::Core::UUID, entt::entity> m_EntityMap;

    entt::registry m_Registry;
    glm::vec2 m_ViewportPosition = { 0.0f, 0.0f };
    float m_ViewportWidth = 1080, m_ViewportHeight = 720;

    friend class SceneRenderer;
    friend class Entity;
    friend class SceneSerializer;
    friend class SceneAssetLoader;
};

}
