#pragma once

#include <entt.hpp>

namespace TerranEngine 
{
	class Scene 
	{
	public:
		Scene();

		entt::entity CreateEntity();

		template<typename... Args>
		void AddComponent(Args&&... components, entt::entity& handle) 
		{

		}

		template<typename Component>
		bool HasComponent(Component component, entt::entity& handle)
		{
			return m_Registry.all_of<component>(handle);
		}

		void Update();
	private:
		entt::registry m_Registry;

	};
}