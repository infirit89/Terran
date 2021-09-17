#pragma once

#include <entt.hpp>

namespace TerranEngine 
{
	class Scene 
	{
	public:
		Scene();

		entt::entity CreateEntity();

		template<typename Component, typename... Args>
		void AddComponent(entt::entity& handle, Args&&... parameters)
		{
			TR_ASSERT(!HasComponent<Component>(handle), "Entity already has component");

			m_Registry.emplace<Component>(handle, std::forward<Args>(parameters)...);
		}

		template<typename T>
		T& GetComponent(entt::entity& handle) 
		{
			TR_ASSERT(HasComponent<T>(handle), "Entity doesn't have the component");
			return m_Registry.get<T>(handle);
		}

		template<typename Component>
		void RemoveComponent(entt::entity& handle) 
		{
			TR_ASSERT(HasComponent<Component>(handle), "Entity doesn't have component");
			m_Registry.remove<Component>(handle);
		}

		template<typename Component>
		bool HasComponent(entt::entity& handle)
		{
			return m_Registry.all_of<Component>(handle);
		}

		void Update();
		entt::registry m_Registry;
	};
}