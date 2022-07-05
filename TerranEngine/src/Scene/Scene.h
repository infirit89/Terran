#pragma once

#include "Core/UUID.h"
#include "Core/Time.h"
#include "Core/Base.h"

#include "Graphics/Camera.h"

#pragma warning(push)
#pragma warning(disable : 26439)
#include <entt.hpp>
#pragma warning(pop)

#include <unordered_map>

namespace TerranEngine 
{
	class Entity;
	class SceneRenderer;
	
	class Scene 
	{
	public:
		
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(const std::string name, const UUID& uuid);

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

		Entity GetPrimaryCamera();

		Entity DuplicateEntity(Entity srcEntity, Entity parent);
		Entity DuplicateEntity(Entity srcEntity);

		static Shared<Scene> CopyScene(Shared<Scene>& srcScene);

		UUID& GetID() { return m_ID; }

		bool IsPlaying() const { return m_IsPlaying; }

		Scene* GetRaw() { return this; }

		static Shared<Scene> GetScene(const UUID& id);
		
	private:
		void OnScriptComponentConstructed(entt::registry& registry, entt::entity entityHandle);
		void OnScriptComponentDestroyed(entt::registry& registry, entt::entity entityHandle);

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