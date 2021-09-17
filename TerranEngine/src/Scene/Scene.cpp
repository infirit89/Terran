#include "trpch.h"
#include "Scene.h"

#include "Components.h"
#include "Renderer/BatchRenderer2D.h"

namespace TerranEngine 
{
	Scene::Scene()
	{
	}

	entt::entity Scene::CreateEntity()
	{
		auto entity = m_Registry.create();
		AddComponent<TransformComponent>(entity);

		return entity;
	}

	void Scene::Update()
	{
		auto cameraView = m_Registry.view<CameraComponent>();
		
		Camera camera;
		glm::mat4 cameraTransform;

		for (auto e : cameraView)
		{
			auto& transformComponent = GetComponent<TransformComponent>(e);
			auto& cameraComponent = GetComponent<CameraComponent>(e);

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
			auto& transformComponent = GetComponent<TransformComponent>(e);
			auto& srComponent = GetComponent<SpriteRendererComponent>(e);

			BatchRenderer2D::Get()->AddQuad(transformComponent.GetTransformMatrix(), srComponent.Color);
		}

		BatchRenderer2D::Get()->EndScene();
	}
}
