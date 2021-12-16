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
			TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

			TR_ASSERT(!HasComponent<Component>(), "Entity: {0} already has component", GetName());

			Component& component = m_Scene->m_Registry.emplace<Component>(m_Handle, std::forward<Args>(parameters)...);
			return component;
		}

		template<typename Component>
		inline Component& GetComponent() const
		{
			TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

			TR_ASSERT(HasComponent<Component>(), "Entity doesn't have the component");
			return m_Scene->m_Registry.get<Component>(m_Handle);
		}

		template<typename Component>
		inline void RemoveComponent()
		{
			TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

			TR_ASSERT(HasComponent<Component>(), "Entity doesn't have component");
			m_Scene->m_Registry.remove<Component>(m_Handle);
		}

		template<typename Component>
		inline Component& TryGetComponent() 
		{
			TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

			return m_Scene->m_Registry.try_get<Component>(m_Handle);
		}
		
		template<typename Component>
		inline bool HasComponent() const
		{
			TR_ASSERT(m_Handle != entt::null, "Ivalid entity");

			return m_Scene->m_Registry.all_of<Component>(m_Handle);
		}
		
		// base stuffs
		inline UUID& GetID()										{ return GetComponent<TagComponent>().ID; }
		const inline bool Valid() const								{ return m_Scene->m_Registry.valid(m_Handle); }
		const inline UUID& GetID() const							{ return GetComponent<TagComponent>().ID; }
		inline std::string& GetName()								{ return HasComponent<TagComponent>() ? GetComponent<TagComponent>().Name : ErrorName; }
		const inline std::string& GetName() const					{ return HasComponent<TagComponent>() ? GetComponent<TagComponent>().Name : ErrorName; }
		inline TransformComponent& GetTransform()					{ return GetComponent<TransformComponent>(); }
		const inline TransformComponent& GetTransform() const		{ return GetComponent<TransformComponent>(); }
		inline glm::mat4& GetTransformMat()							{ return GetTransform().TransformMatrix; }
		const inline glm::mat4& GetTransformMat() const				{ return GetTransform().TransformMatrix; }

		// operators
		inline operator bool() const								{ return m_Handle != entt::null; }
		inline operator uint32_t() const							{ return uint32_t(m_Handle); }
		inline operator entt::entity() const						{ return m_Handle; }
		inline bool operator==(const Entity& other) const			{ return m_Handle == other.m_Handle && m_Scene == other.m_Scene; }
		inline bool operator!=(const Entity& other) const			{ return !(*this == other); }

		// relationship component sutffs
		inline bool HasParent()										{ return HasComponent<RelationshipComponent>() ? m_Scene->FindEntityWithUUID(GetComponent<RelationshipComponent>().ParentID) : false; }
		const inline bool HasParent() const							{ return HasComponent<RelationshipComponent>() ? m_Scene->FindEntityWithUUID(GetComponent<RelationshipComponent>().ParentID) : false; }
		const inline UUID& GetParentID() const						{ return HasComponent<RelationshipComponent>() ? GetComponent<RelationshipComponent>().ParentID : UUID::Empty(); }
		const inline size_t GetChildCount() const					{ return HasComponent<RelationshipComponent>() ? GetComponent<RelationshipComponent>().Children.size() : -1; }
		inline std::vector<UUID>& GetChildren() const				{ return GetComponent<RelationshipComponent>().Children; }

		
		inline Entity GetChild(uint32_t index) const		
		{ 
			if (!HasComponent<RelationshipComponent>())
				return { };

			return m_Scene->FindEntityWithUUID(GetChildren()[index]); 
		}

		inline void SetParentID(UUID& id) 
		{
			if (!HasComponent<RelationshipComponent>())
				return;

			auto& relComp = GetComponent<RelationshipComponent>();
			relComp.ParentID = id;
		}

		inline Entity GetParent() const 
		{
			if (!HasComponent<RelationshipComponent>())
				return {};

			return m_Scene->FindEntityWithUUID(GetParentID());
		}

		void SetParent(Entity parent) 
		{
			if (!HasComponent<RelationshipComponent>())
				 AddComponent<RelationshipComponent>();

			if (!parent.HasComponent<RelationshipComponent>())
				parent.AddComponent<RelationshipComponent>();

			auto& relComp = GetComponent<RelationshipComponent>();
			relComp.ParentID = parent.GetID();
			parent.GetChildren().emplace_back(GetID());
		}
		
		void AddChild(Entity child) 
		{

			if (!HasComponent<RelationshipComponent>())
				AddComponent<RelationshipComponent>();

			if (!child.HasComponent<RelationshipComponent>())
				child.AddComponent<RelationshipComponent>();

			auto& relComp = child.GetComponent<RelationshipComponent>();
			relComp.ParentID = GetID();
			GetChildren().emplace_back(child.GetID());
		}

		void RemoveChildFrom(Entity parent, Entity child, bool removeRelationship) 
		{
			if (!parent.HasComponent<RelationshipComponent>())
				return;

			if (!child.HasComponent<RelationshipComponent>())
				return;

			auto& it = std::find(parent.GetChildren().begin(), parent.GetChildren().end(), child.GetID());

			if (it != parent.GetChildren().end())
				parent.GetChildren().erase(it);

			if (removeRelationship)
				child.RemoveComponent<RelationshipComponent>();
		}

		void RemoveChild(Entity child, bool removeRelationship) 
		{
			RemoveChildFrom(*this, child, removeRelationship);
		}

		void Reparent(Entity previousParent, Entity newParent) 
		{
			RemoveChildFrom(previousParent, *this, false);
			SetParent(newParent);
		}

	private:
		entt::entity m_Handle { entt::null };
		Scene* m_Scene;
		std::string ErrorName = "Unnamed";
	};
}
