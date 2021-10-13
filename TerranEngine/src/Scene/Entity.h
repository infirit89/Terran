#pragma once

#include "Scene.h"
#include "Components.h"

namespace TerranEngine 
{
	class Entity 
	{
	public:
		Entity() = default;
		Entity(const entt::entity& handle, Scene* scene) 
			: m_Handle(handle), m_Scene(scene)
		{
		}
		~Entity() = default;

		template<typename Component, typename... Args>
		inline Component& AddComponent(Args&&... parameters)
		{
			TR_ASSERT(!HasComponent<Component>(), "Entity already has component");

			Component& component = m_Scene->m_Registry.emplace<Component>(m_Handle, std::forward<Args>(parameters)...);
			return component;
		}

		template<typename T>
		inline T& GetComponent() const
		{
			TR_ASSERT(HasComponent<T>(), "Entity doesn't have the component");
			return m_Scene->m_Registry.get<T>(m_Handle);
		}

		template<typename Component>
		inline void RemoveComponent()
		{
			TR_ASSERT(HasComponent<Component>(), "Entity doesn't have component");
			m_Scene->m_Registry.remove<Component>(m_Handle);
		}

		template<typename Component>
		inline bool HasComponent() const
		{
			return m_Scene->m_Registry.all_of<Component>(m_Handle);
		}

		inline TransformComponent& GetTransform()
		{
			return GetComponent<TransformComponent>();
		}

		inline UUID& GetID()
		{
			return GetComponent<TagComponent>().ID;
		}

		inline operator uint32_t() const { return uint32_t(m_Handle); }

		inline bool operator==(const Entity& other) const 
		{
			return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
		}

		inline bool operator!=(const Entity& other) const 
		{
			return !(*this == other);
		}

		inline operator entt::entity() const { return m_Handle; }

		inline operator bool() const { return m_Handle != entt::null; }

	private:
		entt::entity m_Handle { entt::null };
		Scene* m_Scene;
	};
}