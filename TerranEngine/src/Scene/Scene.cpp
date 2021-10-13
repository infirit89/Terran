#include "trpch.h"
#include "Scene.h"

#include "Components.h"
#include "Graphics/BatchRenderer2D.h"
#include "Entity.h"
#include "Utils/ResourceManager.h"

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
		m_Registry.destroy(entity);
	}

	void Scene::Update()
	{
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
				cameraTransform = transformComponent.GetTransformMatrix();
			}
		}

		auto srView = m_Registry.view<SpriteRendererComponent>();

		BatchRenderer2D::Get()->BeginScene(camera, cameraTransform);
		for (auto e : srView)
		{
			Entity entity(e, this);

			auto& transformComponent = entity.GetComponent<TransformComponent>();
			auto& srComponent = entity.GetComponent<SpriteRendererComponent>();

			BatchRenderer2D::Get()->AddQuad(transformComponent.GetTransformMatrix(), srComponent.Color);
		}

		auto crView = m_Registry.view<CircleRendererComponent>();

		for (auto e : crView)
		{
			Entity entity(e, this);
			auto& transformComponent = entity.GetComponent<TransformComponent>();
			auto& crComponent = entity.GetComponent<CircleRendererComponent>();

			BatchRenderer2D::Get()->AddCircle(transformComponent.GetTransformMatrix(), crComponent.Color, crComponent.Thickness);
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
