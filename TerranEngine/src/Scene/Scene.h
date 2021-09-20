#pragma once

#include <entt.hpp>


namespace TerranEngine 
{
	class Entity;

	class Scene 
	{
	public:
		Scene();

		Entity CreateEntity();

		void Update();
		void OnResize(float width, float height);

	private:
		entt::registry m_Registry;
		friend class Entity;
	};
}