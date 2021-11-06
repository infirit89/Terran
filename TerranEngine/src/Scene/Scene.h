#pragma once

#include <entt.hpp>

namespace TerranEngine 
{
	class Entity;

	class Scene 
	{
	public:
		Scene();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(const std::string name, const UUID& uuid);

		void DestroyEntity(Entity entity);

		void Update();
		void OnResize(float width, float height);

		// maybe move this method into the entity class ?
		void AddChild(Entity parent, Entity child);

		entt::registry& GetRegistry() { return m_Registry; }

		Entity GetPrimaryCamera();

	private:
		entt::registry m_Registry;
		friend class Entity;
		friend class SceneSerializer;
	};
}