#pragma once

#include <entt.hpp>
#include "Core/UUID.h"
#include <unordered_map>

#include "Graphics/Camera.h"

namespace TerranEngine 
{
	class Entity;
	class SceneRenderer;
	class TransformSystem;

	class Scene 
	{
	public:
		Scene();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(const std::string name, const UUID& uuid);

		void DestroyEntity(Entity entity, bool first);

		void StartRuntime();
		void StopRuntime();

		void Update();
		void UpdateEditor();
		void OnResize(float width, float height);

		void OnRender(Shared<SceneRenderer>& sceneRenderer);
		void OnRenderEditor(Shared<SceneRenderer>& sceneRenderer, Camera& camera, glm::mat4& cameraView);

		Entity FindEntityWithUUID(const UUID& uuid);
		Entity FindEntityWithName(const std::string& name);

		template <typename... Args, typename... Exclude>
		auto GetEntitiesWith(entt::exclude_t<Exclude...> exclude = {}) { return m_Registry.view<Args...>(exclude); }
		
		std::unordered_map<UUID, entt::entity>& GetEntityMap() { return m_EntityMap; }

		Entity GetPrimaryCamera();

		Entity DuplicateEntity(Entity srcEntity);

		static Shared<Scene> CopyScene(Shared<Scene>& srcScene);

	private:
		
	private:
		// TODO: add scene name and UUID

		bool m_RuntimeStarted = false;

		std::unordered_map<UUID, entt::entity> m_EntityMap;

		entt::registry m_Registry;

		friend class SceneRenderer;
		friend class Entity;
		friend class SceneSerializer;
		friend class TransformSystem;
	};
}