#pragma once

#include <entt.hpp>


namespace TerranEngine 
{
	class Entity;

	class Scene 
	{
	public:
		Scene();

		Entity CreateEntity(const std::string& name = "Entity");
		Entity CreateEntityWithUUID(const std::string name, const UUID& uuid);

		void Update();
		void OnResize(float width, float height);

	private:
		entt::registry m_Registry;
		friend class Entity;
		friend class SceneSerializer;
	};
}