#pragma once

#include "Core/UUID.h"
#include "Core/Time.h"
#include "Core/Base.h"

#include "Assets/Asset.h"

#include "Graphics/Camera.h"

#pragma warning(push)
#pragma warning(disable : 26439)
#include <entt.hpp>
#pragma warning(pop)

#include <unordered_map>
#include <vector>
#include <type_traits>

namespace TerranEngine 
{
	class Entity;
	class SceneRenderer;
	
	class Scene : public Asset
	{
	public:
		Scene();
		virtual ~Scene() override;

		ASSET_CLASS_TYPE(Scene)

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(const std::string name, const UUID& uuid);
		Entity CreateEmptyEntity();

		void DestroyEntity(Entity entity, bool first);

		void StartRuntime();
		void StopRuntime();

		void Update(Time time);
		void UpdateEditor();
		void OnResize(float width, float height);

		void OnRender(Shared<SceneRenderer>& sceneRenderer);
		void OnRenderEditor(Shared<SceneRenderer>& sceneRenderer, Camera& camera, glm::mat4& cameraView);

		Entity FindEntityWithUUID(UUID uuid);
		Entity FindEntityWithName(const std::string& name);

		template <typename... Args, typename... Exclude>
		auto GetEntitiesWith(entt::exclude_t<Exclude...> exclude = {}) { return m_Registry.view<Args...>(exclude); }
		
		std::unordered_map<UUID, entt::entity>& GetEntityMap() { return m_EntityMap; }

		template<typename... Components, typename Predicate>
		std::vector<Entity> Filter(Predicate&& predicate) 
		{
			std::vector<Entity> entities;
			auto view = m_Registry.template view<std::add_const_t<Components>...>();
			entities.reserve(view.size());

			for (const entt:handle e : view) 
			{
				if (predicate(view.template get<std::add_const_t<Components>>(e)...))
					entities.push_back({ e, this });
			}

			return entities;
		}

		Entity GetPrimaryCamera();

		Entity DuplicateEntity(Entity srcEntity, Entity parent);
		Entity DuplicateEntity(Entity srcEntity);

		static Shared<Scene> CopyScene(Shared<Scene>& srcScene);

		UUID& GetID() { return m_ID; }

		bool IsPlaying() const { return m_IsPlaying; }

		Scene* GetRaw() { return this; }

		void UpdateTransformHierarchy();
		void UpdateEntityTransform(Entity entity);

		void ConvertToLocalSpace(Entity entity);
		void ConvertToWorldSpace(Entity entity);
	
		void SortEntities();
	private:

        // scripting components
		void OnScriptComponentConstructed(entt::registry& registry, entt::entity entityHandle);
		void OnScriptComponentDestroyed(entt::registry& registry, entt::entity entityHandle);

        // phyiscs components
		void OnRigidbody2DComponentConstructed(entt::registry& registry, entt::entity entityHandle);
		void OnRigidbody2DComponentDestroyed(entt::registry& registry, entt::entity entityHandle);
		
		void OnBoxCollider2DComponentConstructed(entt::registry& registry, entt::entity entityHandle);
		void OnBoxCollider2DComponentDestroyed(entt::registry& registry, entt::entity entityHandle);

		void OnCircleCollider2DComponentConstructed(entt::registry& registry, entt::entity entityHandle);
		void OnCircleCollider2DComponentDestroyed(entt::registry& registry, entt::entity entityHandle);

		void OnCapsuleCollider2DComponentConstructed(entt::registry& registry, entt::entity entityHandle);
		void OnCapsuleCollider2DComponentDestroyed(entt::registry& registry, entt::entity entityHandle);
	private:
		// TODO: add scene name and UUID

		UUID m_ID;

		bool m_IsPlaying = false;

		std::unordered_map<UUID, entt::entity> m_EntityMap;

		entt::registry m_Registry;

		friend class SceneRenderer;
		friend class Entity;
		friend class SceneSerializer;
	};
}
