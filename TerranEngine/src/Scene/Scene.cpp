#include "trpch.h"
#include "Scene.h"

#include "Components.h"
#include "Entity.h"

#include "Graphics/BatchRenderer2D.h"

#include "Systems/TransformSystem.h"
#include "Systems/SceneRenderer.h"

#include "Scripting/ScriptEngine.h"

#include "Physics/PhysicsEngine.h"

#include "Utils/Debug/Profiler.h"
#include "Utils/ResourceManager.h"

namespace TerranEngine 
{

	Scene::Scene()
	{
		m_ID = UUID();

		m_Registry.on_construct<ScriptComponent>().connect<&Scene::OnScriptComponentConstructed>(this);

		m_Registry.on_destroy<ScriptComponent>().connect<&Scene::OnScriptComponentDestroyed>(this);
	}

	Scene::~Scene()
	{
		auto scriptView = m_Registry.view<ScriptComponent>();

		for (auto e : scriptView)
		{
			Entity entity(e, this);

			ScriptEngine::UninitalizeScriptable(entity);
		}

		m_Registry.on_construct<ScriptComponent>().disconnect<&Scene::OnScriptComponentConstructed>(this);

		m_Registry.on_destroy<ScriptComponent>().disconnect<&Scene::OnScriptComponentDestroyed>(this);
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
		ScriptEngine::UninitalizeScriptable(entity);

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

	void Scene::StartRuntime()
	{
		m_RuntimeStarted = true;

		PhysicsEngine::CreatePhysicsWorld({ 0.0f, -9.8f });

		auto rigidbodyView = m_Registry.view<Rigidbody2DComponent>();

		for (auto e: rigidbodyView)
		{
			Entity entity(e, this);

			PhysicsEngine::CreateRigidbody(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				PhysicsEngine::CreateBoxCollider(entity);
			else if (entity.HasComponent<CircleCollider2DComponent>())
				PhysicsEngine::CreateCircleCollider(entity);
		}

		auto scriptbleComponentView = m_Registry.view<ScriptComponent>();

		for (auto e : scriptbleComponentView)
		{
			Entity entity(e, this);
			//ScriptEngine::InitializeScriptable(entity);
			ScriptEngine::StartScriptable(entity);
		}
	}

	void Scene::StopRuntime()
	{
		m_RuntimeStarted = false;
		PhysicsEngine::CleanUpPhysicsWorld();
	}

	void Scene::Update(Time time)
	{
		TR_PROFILE_FUNCN("Scene::Update");

		auto scriptableComponentView = m_Registry.view<ScriptComponent>();

		PhysicsEngine::Update(time);

		for (auto e : scriptableComponentView)
		{
			Entity entity(e, this);
			// NOTE: in case an entity is created during the runtime we want to start that entity's script
			if (!entity.GetComponent<ScriptComponent>().Started) 
			{
				ScriptEngine::InitializeScriptable(entity);
				ScriptEngine::StartScriptable(entity);
			}

			ScriptEngine::UpdateScriptable(entity);
		}

		m_Registry.sort<TransformComponent>([](const auto& lEntity, const auto& rEntity) 
		{ return lEntity.IsDirty && !rEntity.IsDirty; });

		TransformSystem::Update(this);
	}

	void Scene::UpdateEditor()
	{
		m_Registry.sort<TransformComponent>([](const auto& lEntity, const auto& rEntity)
		{ return lEntity.IsDirty && !rEntity.IsDirty; });

		TransformSystem::Update(this);
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

	Entity Scene::FindEntityWithUUID(UUID uuid)
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

	template<typename Component>
	static void CopyComponent(entt::entity srcHandle, entt::entity dstHandle, entt::registry& srcRegistry, entt::registry& dstRegistry) 
	{
		if (srcRegistry.all_of<Component>(srcHandle)) 
		{
			dstRegistry.emplace_or_replace<Component>(dstHandle, srcRegistry.get<Component>(srcHandle));
		}
	}

	template<typename Component>
	static void CopyComponent(entt::entity srcHandle, entt::entity dstHandle, entt::registry& srcRegistry)
	{
		CopyComponent<Component>(srcHandle, dstHandle, srcRegistry, srcRegistry);
	}

	Entity Scene::DuplicateEntity(Entity srcEntity, Entity parent)
	{
		Entity dstEntity = CreateEntity(srcEntity.GetName() + " Copy");

		CopyComponent<TransformComponent>(srcEntity, dstEntity, m_Registry);
		CopyComponent<CameraComponent>(srcEntity, dstEntity, m_Registry);
		CopyComponent<SpriteRendererComponent>(srcEntity, dstEntity, m_Registry);
		CopyComponent<CircleRendererComponent>(srcEntity, dstEntity, m_Registry);
		CopyComponent<ScriptComponent>(srcEntity, dstEntity, m_Registry);
		CopyComponent<Rigidbody2DComponent>(srcEntity, dstEntity, m_Registry);
		CopyComponent<BoxCollider2DComponent>(srcEntity, dstEntity, m_Registry);
		CopyComponent<CircleCollider2DComponent>(srcEntity, dstEntity, m_Registry);

		if (srcEntity.HasComponent<RelationshipComponent>()) 
		{
			// TODO: copy children
			for (int i = 0; i < srcEntity.GetChildCount(); i++)
			{
				Entity childEntity = srcEntity.GetChild(i);
				DuplicateEntity(childEntity, dstEntity);
			}

			if(!parent)
				parent = srcEntity.GetParent();

			if(parent)
				dstEntity.SetParent(parent);
		}

		if (dstEntity.HasComponent<ScriptComponent>()) 
		{
			ScriptEngine::InitializeScriptable(dstEntity);

			if (m_RuntimeStarted)
				ScriptEngine::StartScriptable(dstEntity);

		}
		return dstEntity;
	}

	Entity Scene::DuplicateEntity(Entity srcEntity)
	{
		return DuplicateEntity(srcEntity, {});
	}

	Shared<Scene> Scene::CopyScene(Shared<Scene>& srcScene)
	{
		ScriptEngine::SetCurrentFieldStates(srcScene->GetID());

		Shared<Scene> scene = CreateShared<Scene>();

		auto tagView = srcScene->GetEntitiesWith<TagComponent>();

		for (auto e : tagView)
		{
			Entity srcEntity(e, srcScene.get());

			Entity dstEntity = scene->CreateEntityWithUUID(srcEntity.GetName(), srcEntity.GetID());
		}

		for (auto e : tagView)
		{
			Entity srcEntity(e, srcScene.get());
			Entity dstEntity = scene->FindEntityWithUUID(srcEntity.GetID());

			CopyComponent<TransformComponent>(srcEntity, dstEntity, srcScene->m_Registry, scene->m_Registry);
			CopyComponent<CameraComponent>(srcEntity, dstEntity, srcScene->m_Registry, scene->m_Registry);
			CopyComponent<SpriteRendererComponent>(srcEntity, dstEntity, srcScene->m_Registry, scene->m_Registry);
			CopyComponent<CircleRendererComponent>(srcEntity, dstEntity, srcScene->m_Registry, scene->m_Registry);
			CopyComponent<RelationshipComponent>(srcEntity, dstEntity, srcScene->m_Registry, scene->m_Registry);
			CopyComponent<ScriptComponent>(srcEntity, dstEntity, srcScene->m_Registry, scene->m_Registry);
			CopyComponent<Rigidbody2DComponent>(srcEntity, dstEntity, srcScene->m_Registry, scene->m_Registry);
			CopyComponent<BoxCollider2DComponent>(srcEntity, dstEntity, srcScene->m_Registry, scene->m_Registry);
			CopyComponent<CircleCollider2DComponent>(srcEntity, dstEntity, srcScene->m_Registry, scene->m_Registry);
		}

		ScriptEngine::ClearFieldBackupMap();

		return scene;
	}

	void Scene::OnScriptComponentConstructed(entt::registry& registry, entt::entity entityHandle)
	{
		Entity entity(entityHandle, this);

		ScriptEngine::InitializeScriptable(entity);
	}

	void Scene::OnScriptComponentDestroyed(entt::registry& registry, entt::entity entityHandle)
	{
		Entity entity(entityHandle, this);

		ScriptEngine::UninitalizeScriptable(entity);
	}
}
