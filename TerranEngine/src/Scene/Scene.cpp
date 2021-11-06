#include "trpch.h"
#include "Scene.h"

#include "Components.h"
#include "RelationshipComponent.h"
#include "Entity.h"
#include "Graphics/BatchRenderer2D.h"
#include "Utils/ResourceManager.h"

#include "Systems/TransformSystem.h"

namespace TerranEngine 
{
	Scene::Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(name, UUID());
	}

	Entity Scene::CreateEntityWithUUID(const std::string name, const UUID& uuid)
	{
		entt::entity e = m_Registry.create();

		Entity entity(e, this);
		entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name, uuid);
		entity.AddComponent<TransformComponent>();
		
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		if (entity.HasComponent<RelationshipComponent>()) 
		{
			auto& relationshipComponent = entity.GetComponent<RelationshipComponent>();

			if (relationshipComponent.Parent) 
			{
				Entity& parent = relationshipComponent.Parent;

				parent.GetComponent<RelationshipComponent>().Children--;

				if (parent.GetComponent<RelationshipComponent>().Children > 0) 
				{
					Entity& firstChild = relationshipComponent.Parent.GetComponent<RelationshipComponent>().FirstChild;

					if (firstChild == entity)
						firstChild = relationshipComponent.Next;
					else
						relationshipComponent.Previous = relationshipComponent.Next;
				}
				else 
					parent.RemoveComponent<RelationshipComponent>();
			}

			Entity currChild = relationshipComponent.FirstChild;

			for (size_t i = 0; i < relationshipComponent.Children; i++)
			{
				RelationshipComponent& tempRelComp = currChild.GetComponent<RelationshipComponent>();
				m_Registry.destroy(currChild);

				currChild = tempRelComp.Next;
			}
		}

		m_Registry.destroy(entity);
	}

	void Scene::Update()
	{
		m_Registry.sort<TransformComponent>([](const auto& lEntity, const auto& rEntity) 
		{ return lEntity.Dirty && !rEntity.Dirty; });

		TransformSystem::Update(this);

		m_Registry.sort<SpriteRendererComponent>([](const auto& lEntity, const auto& rEntity) 
		{ return lEntity.ZIndex < rEntity.ZIndex; });

		auto cameraView = m_Registry.view<CameraComponent>();
		
		Camera camera;
		glm::mat4 cameraTransform;

		for (auto e : cameraView)
		{
			Entity entity(e, this);

			auto& transformComponent = entity.GetComponent<TransformComponent>();
			auto& cameraComponent = entity.GetComponent<CameraComponent>();

			if (cameraComponent.Primary) 
			{
				camera = cameraComponent.Camera;
				cameraTransform = transformComponent.TransformMatrix;
			}
		}

		auto srView = m_Registry.view<SpriteRendererComponent>();

		BatchRenderer2D::Get()->BeginScene(camera, cameraTransform);
		for (auto e : srView)
		{
			Entity entity(e, this);

			auto& transformComponent = entity.GetComponent<TransformComponent>();
			auto& srComponent = entity.GetComponent<SpriteRendererComponent>();

			BatchRenderer2D::Get()->AddQuad(transformComponent.TransformMatrix, srComponent.Color);
		}

		auto crView = m_Registry.view<CircleRendererComponent>();

		for (auto e : crView)
		{
			Entity entity(e, this);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			auto& crComponent = entity.GetComponent<CircleRendererComponent>();

			BatchRenderer2D::Get()->AddCircle(transformComponent.TransformMatrix, crComponent.Color, crComponent.Thickness);
		}

		BatchRenderer2D::Get()->EndScene();
	}

	void Scene::OnResize(float width, float height)
	{
		auto cameraView = m_Registry.view<CameraComponent>();

		for (auto e : cameraView)
		{
			Entity entity(e, this);

			auto& cameraComponent = entity.GetComponent<CameraComponent>();

			cameraComponent.Camera.SetViewport(width, height);
		}
	}

	void Scene::AddChild(Entity parent, Entity child)
	{
		child.AddComponent<RelationshipComponent>().Parent = parent;
		if (!parent.HasComponent<RelationshipComponent>())
		{
			RelationshipComponent& relationshipComponent = parent.AddComponent<RelationshipComponent>();
			relationshipComponent.FirstChild = child;
			relationshipComponent.Children++;
			relationshipComponent.LastChild = child;
		}
		else
		{
			RelationshipComponent& relationshipComponent = parent.GetComponent<RelationshipComponent>();

			if (!relationshipComponent.FirstChild)
				relationshipComponent.FirstChild = child;

			relationshipComponent.Children++;

			Entity previousEntity;

			if (relationshipComponent.LastChild)
			{
				previousEntity = relationshipComponent.LastChild;
				previousEntity.GetComponent<RelationshipComponent>().Next = child;
				child.GetComponent<RelationshipComponent>().Previous = previousEntity;
			}

			relationshipComponent.LastChild = child;
		}
	}

	Entity Scene::GetPrimaryCamera()
	{
		auto cameraView = m_Registry.view<CameraComponent>();

		for (auto e : cameraView)
		{
			Entity entity(e, this);

			auto& cameraComponent = entity.GetComponent<CameraComponent>();

			if (cameraComponent.Primary)
				return entity;
		}

		return Entity(entt::entity{ entt::null }, this);
	}
}
