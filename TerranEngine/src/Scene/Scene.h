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

		void Update();
		void OnResize(float width, float height);

		entt::registry& GetRegistry() { return m_Registry; }

		Entity GetPrimaryCamera();

	private:
		entt::registry m_Registry;
		friend class Entity;
		friend class SceneSerializer;
	};
}