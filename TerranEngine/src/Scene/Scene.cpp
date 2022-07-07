#include "trpch.h"
#include "Scene.h"

#include "Components.h"
#include "Entity.h"

#include "Graphics/BatchRenderer2D.h"

#include "Systems/TransformSystem.h"
#include "Systems/SceneRenderer.h"

#include "Scripting/ScriptEngine.h"

#include "Physics/Physics.h"
#include "Scripting/ScriptCache.h"

#include "Utils/Debug/Profiler.h"

namespace TerranEngine 
{

	static std::unordered_map<UUID, Shared<Scene>> s_SceneMap;
	
	struct SceneComponent
	{
		UUID SceneID;
	};
	
	Scene::Scene()
	{
		m_ID = UUID();

		m_Registry.on_construct<ScriptComponent>().connect<&Scene::OnScriptComponentConstructed>(this);
		m_Registry.on_destroy<ScriptComponent>().connect<&Scene::OnScriptComponentDestroyed>(this);

		const auto sceneEntity = m_Registry.create();
		m_Registry.emplace<SceneComponent>(sceneEntity, m_ID);
	}

	Scene::~Scene()
	{
		m_Registry.on_construct<ScriptComponent>().disconnect<&Scene::OnScriptComponentConstructed>(this);
		m_Registry.on_destroy<ScriptComponent>().disconnect<&Scene::OnScriptComponentDestroyed>(this);
	}

	Shared<Scene> Scene::CreateEmpty()
	{
		Shared<Scene> scene = CreateShared<Scene>();
		s_SceneMap.emplace(scene->m_ID, scene);
		
		return scene;
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

		if (entity.HasComponent<Rigidbody2DComponent>())
			Physics2D::DestroyPhysicsBody(entity);

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
		if(m_IsPlaying)
			return;
		
		m_IsPlaying = true;

		Physics2D::SetContext(GetScene(m_ID));
		Physics2D::CreatePhysicsWorld({ 0.0f, -9.8f });

		auto rigidbodyView = m_Registry.view<Rigidbody2DComponent>();

		for (auto e: rigidbodyView)
		{
			Entity entity(e, this);

			Physics2D::CreatePhysicsBody(entity);
		}


		ScriptEngine::SetContext(GetScene(m_ID));
		auto scriptbleComponentView = m_Registry.view<ScriptComponent>();

		for (auto e : scriptbleComponentView)
		{
			Entity entity(e, this);
			//ScriptEngine::InitializeScriptable(entity);
			ScriptEngine::OnStart(entity);
		}
	}

	void Scene::StopRuntime()
	{
		if(!m_IsPlaying)
			return;
		
		m_IsPlaying = false;
		Physics2D::CleanUpPhysicsWorld();
	}

	void Scene::Update(Time time)
	{
		TR_PROFILE_FUNCN("Scene::Update");
		
		m_Registry.sort<TransformComponent>([](const auto& lEntity, const auto& rEntity) 
		{ return lEntity.IsDirty && !rEntity.IsDirty; });

		TransformSystem::SetContext(this);
		TransformSystem::Update();

		Physics2D::Update(time);

		auto scriptableComponentView = m_Registry.view<ScriptComponent>();
		for (auto e : scriptableComponentView)
		{
			Entity entity(e, this);
			// NOTE: in case an entity is created during the runtime we want to start that entity's script
			if (!entity.GetComponent<ScriptComponent>().Started) 
			{
				ScriptEngine::InitializeScriptable(entity);
				ScriptEngine::OnStart(entity);
			}

			ScriptEngine::OnUpdate(entity);
		}

	}

	void Scene::UpdateEditor()
	{
		m_Registry.sort<TransformComponent>([](const auto& lEntity, const auto& rEntity)
		{ return lEntity.IsDirty && !rEntity.IsDirty; });

		TransformSystem::SetContext(this);
		TransformSystem::Update();
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

			sceneRenderer->BeginScene(camera, cameraTransform, true);
			
			// submit sprites
			{
				auto spriteRendererView = m_Registry.view<SpriteRendererComponent>();
				for (auto e : spriteRendererView) 
				{
					Entity entity(e, this);
					auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();
				
					sceneRenderer->SubmitSprite(spriteRenderer, entity.GetWorldMatrix(), (int)((uint32_t)entity));
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

			// submit text
			{
				auto textRendererView = m_Registry.view<TextRendererComponent>();
				for (auto e : textRendererView)
				{
					Entity entity(e, this);
					auto& textRenderer = entity.GetComponent<TextRendererComponent>();

					sceneRenderer->SubmitText(textRenderer, entity.GetWorldMatrix());
				}
			}

			sceneRenderer->EndScene();
		}
	}
	
	void Scene::OnRenderEditor(Shared<SceneRenderer>& sceneRenderer, Camera& camera, glm::mat4& cameraView)
	{
		sceneRenderer->SetScene(this);
		sceneRenderer->BeginScene(camera, cameraView, false);

		sceneRenderer->GetFramebuffer()->SetColorAttachment(1, -1);

		// submit sprites
		{
			auto spriteRendererView = m_Registry.view<SpriteRendererComponent>();
			for (auto e : spriteRendererView)
			{
				Entity entity(e, this);
				auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();

				sceneRenderer->SubmitSprite(spriteRenderer, entity.GetWorldMatrix(), (int)((uint32_t)entity));
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

		// submit lines
		{
			auto lineRendererView = m_Registry.view<LineRendererComponent>();
			for (auto e : lineRendererView)
			{
				Entity entity(e, this);
				auto& lineRenderer = entity.GetComponent<LineRendererComponent>();

				sceneRenderer->SubmitLine(lineRenderer);
			}
		}

		// submit text
		{
			auto textRendererView = m_Registry.view<TextRendererComponent>();
			for (auto e : textRendererView)
			{
				Entity entity(e, this);
				auto& textRenderer = entity.GetComponent<TextRendererComponent>();

				sceneRenderer->SubmitText(textRenderer, entity.GetWorldMatrix());
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
		const auto tagView = m_Registry.view<TagComponent>();

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
		const auto cameraView = m_Registry.view<CameraComponent>();
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

			if constexpr (std::is_same<Component, ScriptComponent>::value)
			{
				ScriptComponent& sc = dstRegistry.get<ScriptComponent>(dstHandle);

				for (const auto& fieldID : sc.PublicFieldIDs)
				{
					ScriptField* field = ScriptCache::GetCachedFieldFromID(fieldID);

					const entt::entity srcSceneEntity = srcRegistry.view<SceneComponent>().front();
					const UUID& srcSceneID = srcRegistry.get<SceneComponent>(srcSceneEntity).SceneID;

					const entt::entity dstSceneEntity = dstRegistry.view<SceneComponent>().front();
					const UUID& dstSceneID = dstRegistry.get<SceneComponent>(dstSceneEntity).SceneID;

					const UUID& srcEntityID = srcRegistry.get<TagComponent>(srcHandle).ID;
					const UUID& dstEntityID = dstRegistry.get<TagComponent>(dstHandle).ID;
					
					const GCHandle srcEntityHandle = ScriptEngine::GetScriptInstanceGCHandle(srcSceneID, srcEntityID);
					const GCHandle dstEntityHandle = ScriptEngine::GetScriptInstanceGCHandle(dstSceneID, dstEntityID);

					field->CopyData(srcEntityHandle, dstEntityHandle);
				}
			}
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

			if (m_IsPlaying)
				ScriptEngine::OnStart(dstEntity);

		}
		return dstEntity;
	}

	Entity Scene::DuplicateEntity(Entity srcEntity)
	{
		return DuplicateEntity(srcEntity, {});
	}

	Shared<Scene> Scene::CopyScene(Shared<Scene>& srcScene)
	{
		Shared<Scene> scene = CreateEmpty();

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

		return scene;
	}

	Shared<Scene> Scene::GetScene(const UUID& id)
	{
		if(s_SceneMap.find(id) != s_SceneMap.end())
			return s_SceneMap.at(id);

		return nullptr;
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
