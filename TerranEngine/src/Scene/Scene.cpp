#include "trpch.h"
#include "Scene.h"

#include "Components.h"
#include "Entity.h"
#include "Graphics/BatchRenderer2D.h"
#include "Utils/ResourceManager.h"

#include "Systems/TransformSystem.h"
#include "Systems/SceneRenderer.h"

#include "Utils/Debug/Profiler.h"

namespace TerranEngine 
{
	Scene::Scene()
	{
		m_TransformSystem = CreateShared<TransformSystem>(this);
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
		TR_PROFILE_FUNCN("Scene::Update");

		auto scriptableComponentView = m_Registry.view<ScriptableComponent>();

		for (auto e : scriptableComponentView)
		{
			Entity entity(e, this);

			ScriptableComponent& scriptableComponent = entity.GetComponent<ScriptableComponent>();

			scriptableComponent.OnCreate(entity);
			scriptableComponent.OnUpdate();
		}

		m_Registry.sort<TransformComponent>([](const auto& lEntity, const auto& rEntity) 
		{ return lEntity.IsDirty && !rEntity.IsDirty; });

		m_TransformSystem->Update();

	}

	void Scene::UpdateEditor()
	{
		m_Registry.sort<TransformComponent>([](const auto& lEntity, const auto& rEntity)
			{ return lEntity.IsDirty && !rEntity.IsDirty; });

		m_TransformSystem->Update();
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

	void Scene::OnRender(Shared<SceneRenderer>& sceneRenderer)
	{
		Entity primaryCamera = GetPrimaryCamera();

		if (primaryCamera) 
		{
			sceneRenderer->SetScene(this);

			Camera& camera = primaryCamera.GetComponent<CameraComponent>().Camera;
			glm::mat4& cameraTransform = primaryCamera.GetWorldMatrix();

			sceneRenderer->BeginScene(camera, cameraTransform);
			
			// submit sprites
			{
				auto spriteRendererView = m_Registry.view<SpriteRendererComponent>();

				for (auto e : spriteRendererView) 
				{
					Entity entity(e, this);

					auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();
				
					sceneRenderer->SubmitSprite(spriteRenderer, entity.GetWorldMatrix());
				}
			}

			// submit circles
			{
				auto circleRendererView = m_Registry.view<CircleRendererComponent>();

				for (auto e : circleRendererView) 
				{
					Entity entity(e, this);
					auto& circleRenderer = entity.GetComponent<CircleRendererComponent>();

					sceneRenderer->SubmitCircle(circleRenderer, entity.GetWorldMatrix());
				}

			}

			sceneRenderer->EndScene();
		}
	}

	void Scene::OnRenderEditor(Shared<SceneRenderer>& sceneRenderer, Camera& camera, glm::mat4& cameraView)
	{
		sceneRenderer->SetScene(this);
		sceneRenderer->BeginScene(camera, cameraView);
		
		// submit sprites
		{
			auto spriteRendererView = m_Registry.view<SpriteRendererComponent>();

			for (auto e : spriteRendererView)
			{
				Entity entity(e, this);
				auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();

				sceneRenderer->SubmitSprite(spriteRenderer, entity.GetWorldMatrix());
			}
		}

		// submit circles
		{
			auto circleRendererView = m_Registry.view<CircleRendererComponent>();

			for (auto e : circleRendererView)
			{
				Entity entity(e, this);
				auto& circleRenderer = entity.GetComponent<CircleRendererComponent>();

				sceneRenderer->SubmitCircle(circleRenderer, entity.GetWorldMatrix());
			}
		}

		sceneRenderer->EndScene();
	}

	Entity Scene::FindEntityWithUUID(const UUID& uuid)
	{
		if (m_EntityMap.find(uuid) != m_EntityMap.end())
			return Entity(m_EntityMap[uuid], this);

		return { };
	}

	Entity Scene::FindEntityWithName(const std::string& name)
	{
		auto tagView = m_Registry.view<TagComponent>();

		for (auto e : tagView) 
		{
			Entity entity(e, this);
			if (entity.GetName() == name)
				return entity;
		}

		return { };
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

		return { };
	}
}
