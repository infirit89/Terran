#pragma once

#include "Scene.h"
#include "Components.h"

#include "Core/Log.h"
#include "Core/Assert.h"

#pragma warning(push)
#pragma warning(disable : 4834)

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
		inline const UUID& GetID() const							{ return GetComponent<TagComponent>().ID; }
		inline TransformComponent& GetTransform()					{ return GetComponent<TransformComponent>(); }
		inline const TransformComponent& GetTransform() const		{ return GetComponent<TransformComponent>(); }
		inline bool Valid() const									{ return m_Scene->m_Registry.valid(m_Handle); }
		inline std::string& GetName()								{ return GetComponent<TagComponent>().Name; }
		inline const std::string& GetName() const					{ return GetComponent<TagComponent>().Name; }

		// operators
		inline operator entt::entity() const						{ return m_Handle; }
		inline bool operator!=(const Entity& other) const			{ return !(*this == other); }
		inline operator uint32_t() const							{ return uint32_t(m_Handle); }
		inline operator bool() const								{ return m_Handle != entt::null; }
		inline bool operator==(const Entity& other) const			{ return m_Handle == other.m_Handle && m_Scene == other.m_Scene; }

		// relationship component stuffs
		inline std::vector<UUID>& GetChildren()	const				{ return GetComponent<RelationshipComponent>().Children; }
		inline size_t GetChildCount() const							{ return HasComponent<RelationshipComponent>() ? GetComponent<RelationshipComponent>().Children.size() : 0; }
		inline UUID GetParentID() const								{ return HasComponent<RelationshipComponent>() ? GetComponent<RelationshipComponent>().ParentID : UUID::Empty(); }
		inline bool HasParent()										{ return HasComponent<RelationshipComponent>() ? m_Scene->FindEntityWithUUID(GetComponent<RelationshipComponent>().ParentID) : false; }

		inline const UUID& GetSceneID() const						{ return m_Scene->GetID(); }

		inline Entity GetChild(uint32_t index) const		
		{ 
			if (!HasComponent<RelationshipComponent>())
				return { };

			return m_Scene->FindEntityWithUUID(GetChildren()[index]); 
		}

		inline void SetParentID(const UUID& id) 
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

		bool IsChildOf(Entity entity)
		{
			if (!HasComponent<RelationshipComponent>())
				return false;

			if (!entity.HasComponent<RelationshipComponent>())
				return false;

			return GetParentID() == entity.GetID();
		}

		void SetParent(Entity parent, bool forceTransformUpdate = false) 
		{
			if (!HasComponent<RelationshipComponent>())
				 AddComponent<RelationshipComponent>();

			if (!parent.HasComponent<RelationshipComponent>())
				parent.AddComponent<RelationshipComponent>();

			if (IsChildOf(parent)) return;
			if (parent.IsChildOf(*this)) return;

			if (HasParent())
				Unparent();

			auto& relComp = GetComponent<RelationshipComponent>();
			relComp.ParentID = parent.GetID();
			parent.GetChildren().emplace_back(GetID());

			m_Scene->ConvertToLocalSpace(*this);
		}

		void Unparent() 
		{
			if (!HasComponent<RelationshipComponent>())
				return;

			UUID parentID = GetComponent<RelationshipComponent>().ParentID;
			Entity parent = m_Scene->FindEntityWithUUID(parentID);

			if (!parent)
				return;

			m_Scene->ConvertToWorldSpace(*this);

			const auto& it = std::find(parent.GetChildren().begin(), parent.GetChildren().end(), GetID());

			if (it != parent.GetChildren().end())
				parent.GetChildren().erase(it);

			SetParentID(UUID({ 0 }));

			// TODO: if the relationship component is no longer necessary than remove it
		}

		/*void Unparent(Entity parent, Entity child, bool removeRelationship) 
		{
			if (!parent.HasComponent<RelationshipComponent>())
				return;

			if (!child.HasComponent<RelationshipComponent>())
				return;

			const auto& it = std::find(parent.GetChildren().begin(), parent.GetChildren().end(), child.GetID());

			if (it != parent.GetChildren().end())
				parent.GetChildren().erase(it);

			if (removeRelationship)
				child.RemoveComponent<RelationshipComponent>();
			else 
			{
				RelationshipComponent& rc = child.GetComponent<RelationshipComponent>();
				rc.ParentID = UUID({ 0 });
			}
		}*/

		void RemoveChild(Entity child, bool removeRelationship) 
		{
			child.Unparent();
		}

		void Reparent(Entity previousParent, Entity newParent) 
		{
			Unparent();
			SetParent(newParent);
		}

	private:
		entt::entity m_Handle { entt::null };
		Scene* m_Scene = nullptr;
	};
}
#pragma warning(pop)
