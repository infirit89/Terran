#include "trpch.h"
#include "Scene.h"

#include "Components.h"
#include "Entity.h"
#include "Graphics/BatchRenderer2D.h"
#include "Utils/ResourceManager.h"

#include "Systems/TransformSystem.h"

#include "Utils/Debug/DebugTimer.h"

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
		
		m_EntityMap[uuid] = e;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity, bool first)
	{
		//DebugTimer timer;
		if (entity.HasComponent<RelationshipComponent>()) 
		{
			auto& relationshipComponent = entity.GetComponent<RelationshipComponent>();

			if (first) 
			{
				if (entity.HasParent()) 
					entity.GetParent().RemoveChild(entity, false);
			}

			for (auto eID : entity.GetChildren())
				DestroyEntity(FindEntityWithUUID(eID), false);
		}

		auto entityIt = m_EntityMap.find(entity.GetID());
		if (entityIt != m_EntityMap.end())
			m_EntityMap.erase(entityIt);

		m_Registry.destroy(entity);
	}

	void Scene::Update()
	{
		//DebugTimer timer;

		m_Registry.sort<TransformComponent>([](const auto& lEntity, const auto& rEntity) 
		{ return lEntity.Dirty && !rEntity.Dirty; });

		TransformSystem::Update(this);

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
				break;
			}
		}

		m_Registry.sort<SpriteRendererComponent>([](const auto& lEntity, const auto& rEntity) 
		{ return lEntity.ZIndex < rEntity.ZIndex; });

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

	Entity Scene::FindEntityWithUUID(const UUID& uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return Entity(m_EntityMap[uuid], this);

		return { };
	}

	Entity Scene::FindEntityWithName(const std::string& name)
	{
		return Entity();
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
