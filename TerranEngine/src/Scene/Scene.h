#pragma once

#include <entt.hpp>
#include "Core/UUID.h"
#include <unordered_map>

#include "Graphics/Camera.h"

namespace TerranEngine 
{
	class Entity;

	class Scene 
	{
	public:
		Scene();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(const std::string name, const UUID& uuid);

		void DestroyEntity(Entity entity, bool first);

		void Update(const Camera& inCamera = Camera(glm::mat4(0.0f)), const glm::mat4& inCameraTransfrom = glm::mat4(1.0f));
		void OnResize(float width, float height);

		Entity FindEntityWithUUID(const UUID& uuid);
		Entity FindEntityWithName(const std::string& name);

		entt::registry& GetRegistry() { return m_Registry; }

		Entity GetPrimaryCamera();

	private:
		std::unordered_map<UUID, entt::entity> m_EntityMap;
		entt::registry m_Registry;
		friend class Entity;
		friend class SceneSerializer;
	};
}