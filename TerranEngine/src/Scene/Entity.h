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
			: m_Handle(handle), m_Scene(scene) { }

		~Entity() = default;

		template<typename Component, typename... Args>
		inline Component& AddComponent(Args&&... parameters)
		{
			TR_ASSERT(!HasComponent<Component>(), "Entity already has component");

			Component& component = m_Scene->m_Registry.emplace<Component>(m_Handle, std::forward<Args>(parameters)...);
			return component;
		}

		template<typename Component>
		inline Component& GetComponent() const
		{
			TR_ASSERT(HasComponent<Component>(), "Entity doesn't have the component");
			return m_Scene->m_Registry.get<Component>(m_Handle);
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
			//TR_ASSERT(Valid(), "Entity not valid");
			return m_Scene->m_Registry.all_of<Component>(m_Handle);
		}

		inline bool Valid() const							{ return m_Scene->m_Registry.valid(m_Handle); }
		inline UUID& GetID() const							{ return GetComponent<TagComponent>().ID; }
		inline std::string& GetName() const					{ return GetComponent<TagComponent>().Name; }
		inline TransformComponent& GetTransform() const		{ return GetComponent<TransformComponent>(); }

		inline operator bool() const						{ return m_Handle != entt::null; }
		inline operator uint32_t() const					{ return uint32_t(m_Handle); }
		inline operator entt::entity() const				{ return m_Handle; }
		inline bool operator!=(const Entity& other) const	{ return !(*this == other); }
		inline bool operator==(const Entity& other) const	{ return m_Handle == other.m_Handle && m_Scene == other.m_Scene; }

	private:
		entt::entity m_Handle { entt::null };
		Scene* m_Scene;
	};
}