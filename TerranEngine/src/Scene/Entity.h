#pragma once

#include "Scene.h"
#include "Components.h"

namespace TerranEngine 
{
	class Entity 
	{
	public:

		Entity() = default;
		Entity(entt::entity& handle, Scene* scene) 
			: m_Handle(handle), m_Scene(scene)
		{
		}
		~Entity() = default;

		template<typename Component, typename... Args>
		void AddComponent(Args&&... parameters)
		{
			TR_ASSERT(!HasComponent<Component>(), "Entity already has component");

			m_Scene->m_Registry.emplace<Component>(m_Handle, std::forward<Args>(parameters)...);
		}

		template<typename T>
		T& GetComponent()
		{
			TR_ASSERT(HasComponent<T>(), "Entity doesn't have the component");
			return m_Scene->m_Registry.get<T>(m_Handle);
		}

		template<typename Component>
		void RemoveComponent()
		{
			TR_ASSERT(HasComponent<Component>(), "Entity doesn't have component");
			m_Scene->m_Registry.remove<Component>(m_Handle);
		}

		template<typename Component>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<Component>(m_Handle);
		}

		TransformComponent& GetTransform() 
		{
			return GetComponent<TransformComponent>();
		}

	private:
		entt::entity m_Handle = {};
		Scene* m_Scene;
	};
}