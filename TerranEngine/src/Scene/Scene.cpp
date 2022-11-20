#include "trpch.h"
#include "Scene.h"

#include "Components.h"
#include "Entity.h"
#include "SceneManager.h"

#include "Graphics/BatchRenderer2D.h"

#include "Systems/SceneRenderer.h"

#include "Scripting/ScriptEngine.h"
#include "Scripting/ScriptCache.h"

#include "Physics/Physics.h"
#include "Physics/PhysicsBody.h"

#include "Project/Project.h"

#include "Math/Math.h"

#include "Utils/Debug/Profiler.h"
#include "Utils/Debug/OptickProfiler.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace TerranEngine 
{
	struct SceneComponent
	{
		UUID SceneID;
	};
	
	Scene::Scene()
	{
		m_ID = UUID();
        const auto sceneEntity = m_Registry.create();
        m_Registry.emplace<SceneComponent>(sceneEntity, m_ID);

		m_Registry.on_construct<ScriptComponent>().connect<&Scene::OnScriptComponentConstructed>(this);
		m_Registry.on_destroy<ScriptComponent>().connect<&Scene::OnScriptComponentDestroyed>(this);
        
		m_Registry.on_construct<Rigidbody2DComponent>().connect<&Scene::OnRigidbody2DComponentConstructed>(this);
		m_Registry.on_destroy<Rigidbody2DComponent>().connect<&Scene::OnRigidbody2DComponentDestroyed>(this);

		m_Registry.on_construct<BoxCollider2DComponent>().connect<&Scene::OnBoxCollider2DComponentConstructed>(this);
		m_Registry.on_destroy<BoxCollider2DComponent>().connect<&Scene::OnBoxCollider2DComponentDestroyed>(this);

		m_Registry.on_construct<CircleCollider2DComponent>().connect<&Scene::OnCircleCollider2DComponentConstructed>(this);
		m_Registry.on_destroy<CircleCollider2DComponent>().connect<&Scene::OnCircleCollider2DComponentDestroyed>(this);

		m_Registry.on_construct<CapsuleCollider2DComponent>().connect<&Scene::OnCapsuleCollider2DComponentConstructed>(this);
		m_Registry.on_destroy<CapsuleCollider2DComponent>().connect<&Scene::OnCapsuleCollider2DComponentDestroyed>(this);
	}

	Scene::~Scene()
	{
		auto scriptbleComponentView = m_Registry.view<ScriptComponent>();

		for (auto e : scriptbleComponentView)
		{
			Entity entity(e, this);
			ScriptEngine::UninitalizeScriptable(entity);
		}

        m_Registry.clear();

		m_Registry.on_construct<ScriptComponent>().disconnect<&Scene::OnScriptComponentConstructed>(this);
		m_Registry.on_destroy<ScriptComponent>().disconnect<&Scene::OnScriptComponentDestroyed>(this);

		m_Registry.on_construct<Rigidbody2DComponent>().disconnect<&Scene::OnRigidbody2DComponentConstructed>(this);
		m_Registry.on_destroy<Rigidbody2DComponent>().disconnect<&Scene::OnRigidbody2DComponentDestroyed>(this);
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

		SortEntities();
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

		SortEntities();
	}

	void Scene::StartRuntime()
	{
		if(m_IsPlaying)
			return;
		
		m_IsPlaying = true;

        Physics2D::CreatePhysicsWorld(Project::GetPhysicsSettings());
        Physics2D::CratePhysicsBodies(this);

		auto scriptbleComponentView = m_Registry.view<ScriptComponent>();

		for (auto e : scriptbleComponentView)
		{
			Entity entity(e, this);
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
		TR_PROFILE_FUNCTION();

		UpdateTransformHierarchy();

        Physics2D::Update(time);

		auto scriptableComponentView = m_Registry.view<ScriptComponent>();
		for (auto e : scriptableComponentView)
		{
			Entity entity(e, this);
			ScriptEngine::OnUpdate(entity);
		}

	}

	void Scene::UpdateEditor()
	{
		TR_PROFILE_FUNCTION();
		UpdateTransformHierarchy();
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
		TR_PROFILE_FUNCTION();
		Entity primaryCamera = GetPrimaryCamera();

		glm::vec4 backgroundColor = glm::vec4(0.0f);

		if (primaryCamera) 
		{
			backgroundColor = primaryCamera.GetComponent<CameraComponent>().BackgroundColor;
			sceneRenderer->SetScene(this);
			sceneRenderer->SetClearColor(backgroundColor);

			Camera& camera = primaryCamera.GetComponent<CameraComponent>().Camera;
			glm::mat4& cameraTransform = primaryCamera.GetTransform().WorldSpaceTransformMatrix;

			sceneRenderer->BeginScene(camera, cameraTransform, true);
			
			// submit sprites
			{
				auto spriteRendererView = m_Registry.view<SpriteRendererComponent>();
				for (auto e : spriteRendererView) 
				{
					Entity entity(e, this);
					auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();
					auto& transform = entity.GetTransform();
				
					sceneRenderer->SubmitSprite(spriteRenderer, transform.WorldSpaceTransformMatrix, (int)((uint32_t)entity));
				}
			}

			// submit circles
			{
				auto circleRendererView = m_Registry.view<CircleRendererComponent>();
				for (auto e : circleRendererView) 
				{
					Entity entity(e, this);
					auto& circleRenderer = entity.GetComponent<CircleRendererComponent>();
					auto& transform = entity.GetTransform();

					sceneRenderer->SubmitCircle(circleRenderer, transform.WorldSpaceTransformMatrix, (int)(uint32_t)(entity));
				}
			}

			// submit text
			{
				auto textRendererView = m_Registry.view<TextRendererComponent>();
				for (auto e : textRendererView)
				{
					Entity entity(e, this);
					auto& textRenderer = entity.GetComponent<TextRendererComponent>();
					auto& transform = entity.GetTransform();

					sceneRenderer->SubmitText(textRenderer, transform.WorldSpaceTransformMatrix);
				}
			}

			sceneRenderer->EndScene();
		}
	}
	
	void Scene::OnRenderEditor(Shared<SceneRenderer>& sceneRenderer, Camera& camera, glm::mat4& cameraView)
	{
		TR_PROFILE_FUNCTION();
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
				auto& transform = entity.GetTransform();

				sceneRenderer->SubmitSprite(spriteRenderer, transform.WorldSpaceTransformMatrix, 
											(int)((uint32_t)entity));
			}
		}

		// submit circles
		{
			auto circleRendererView = m_Registry.view<CircleRendererComponent>();
			for (auto e : circleRendererView)
			{
				Entity entity(e, this);
				auto& circleRenderer = entity.GetComponent<CircleRendererComponent>();
				auto& transform = entity.GetTransform();

				sceneRenderer->SubmitCircle(circleRenderer, transform.WorldSpaceTransformMatrix, (int)(uint32_t)entity);
			}
		}

		// submit lines
		{
			/*auto lineRendererView = m_Registry.view<LineRendererComponent>();
			for (auto e : lineRendererView)
			{
				Entity entity(e, this);
				auto& lineRenderer = entity.GetComponent<LineRendererComponent>();

				sceneRenderer->SubmitLine(lineRenderer);
			}*/
		}

		// submit text
		{
			auto textRendererView = m_Registry.view<TextRendererComponent>();
			for (auto e : textRendererView)
			{
				Entity entity(e, this);
				auto& textRenderer = entity.GetComponent<TextRendererComponent>();
				auto& transform = entity.GetTransform();

				sceneRenderer->SubmitText(textRenderer, transform.WorldSpaceTransformMatrix);
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
		CopyComponent<CapsuleCollider2DComponent>(srcEntity, dstEntity, m_Registry);

		if (srcEntity.HasComponent<RelationshipComponent>()) 
		{
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

		return dstEntity;
	}

	Entity Scene::DuplicateEntity(Entity srcEntity)
	{
		return DuplicateEntity(srcEntity, {});
	}

	Shared<Scene> Scene::CopyScene(Shared<Scene>& srcScene)
	{
		Shared<Scene> scene = SceneManager::CreateEmpyScene();

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
			CopyComponent<CapsuleCollider2DComponent>(srcEntity, dstEntity, srcScene->m_Registry, scene->m_Registry);
		}

		scene->SortEntities();

		return scene;
	}

	static glm::mat4 CalculateTransformMatrix(TransformComponent& transform)
	{
		return glm::translate(glm::mat4(1.0f), transform.Position) *
			glm::toMat4(glm::quat(transform.Rotation)) *
			glm::scale(glm::mat4(1.0f), transform.Scale);
	}

	void Scene::UpdateTransformHierarchy()
	{
		TR_PROFILE_FUNCN("Scene::UpdateTransformHierarchy");
		TR_PROFILE_FUNCTION();

		auto transformView = GetEntitiesWith<TransformComponent>();

		for (auto e : transformView)
		{ 
			Entity entity(e, this);
			auto& transform = entity.GetTransform();
			
			if (!entity.HasParent())
				UpdateEntityTransform(entity);
		}
	}

	void Scene::UpdateEntityTransform(Entity entity)
	{
		TransformComponent& tc = entity.GetComponent<TransformComponent>();

		if (tc.IsDirty) 
		{
			Entity parent = entity.GetParent();
			if (parent)
			{
				glm::mat4 parentTransform = parent.GetTransform().WorldSpaceTransformMatrix;
				tc.WorldSpaceTransformMatrix = parentTransform * CalculateTransformMatrix(tc);
				tc.LocalSpaceTransformMatrix = glm::inverse(parentTransform) * tc.WorldSpaceTransformMatrix;
			}
			else
			{
				tc.WorldSpaceTransformMatrix = CalculateTransformMatrix(tc);
				tc.LocalSpaceTransformMatrix = tc.WorldSpaceTransformMatrix;
			}

			glm::quat rotation = tc.Rotation;

			tc.Forward = glm::normalize(glm::rotate(rotation, glm::vec3(0.0f, 0.0f, 1.0f)));
			tc.Up = glm::normalize(glm::rotate(rotation, glm::vec3(0.0f, 1.0f, 0.0f)));
			tc.Right = glm::normalize(glm::rotate(rotation, glm::vec3(1.0f, 0.0f, 0.0f)));
		}

		for (size_t i = 0; i < entity.GetChildCount(); i++)
		{
			Entity currEntity = entity.GetChild(i);

			if(tc.IsDirty)
				currEntity.GetTransform().IsDirty = true;

			UpdateEntityTransform(currEntity);
		}

		tc.IsDirty = false;
	}

	void Scene::ConvertToLocalSpace(Entity entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();

		if (!entity.HasParent()) return;

		if (tc.IsDirty)
			UpdateEntityTransform(entity);

		Entity parent = entity.GetParent();
		auto& parentTransform = parent.GetTransform();

		//NOTE: have to calculate it because at this point the local space
		//		and world space transform matrices are equal
		glm::mat4 parentWorldMatrix = parentTransform.WorldSpaceTransformMatrix;
		glm::mat4 localMat = glm::inverse(parentWorldMatrix) * 
										tc.WorldSpaceTransformMatrix;

		Math::Decompose(localMat, tc.Position, tc.Rotation, tc.Scale);

		tc.IsDirty = true;
	}

	void Scene::ConvertToWorldSpace(Entity entity)
	{
		auto& tc = entity.GetComponent<TransformComponent>();

		if (!entity.HasParent()) return;

		if (tc.IsDirty)
			UpdateEntityTransform(entity);

		Math::Decompose(tc.WorldSpaceTransformMatrix, tc.Position, tc.Rotation, tc.Scale);

		tc.IsDirty = true;
	}

	void Scene::SortEntities()
	{
		m_Registry.sort<TagComponent>([](const entt::entity& lEntity, const entt::entity& rEntity)
			{ return lEntity < rEntity; });
	}

	void Scene::OnScriptComponentConstructed(entt::registry& registry, entt::entity entityHandle)
	{
		Entity entity(entityHandle, this);
		ScriptEngine::InitializeScriptable(entity);

        if(m_IsPlaying)
            ScriptEngine::OnStart(entity);
	}

	void Scene::OnScriptComponentDestroyed(entt::registry& registry, entt::entity entityHandle)
	{
		Entity entity(entityHandle, this);
		ScriptEngine::UninitalizeScriptable(entity);
	}


    void Scene::OnRigidbody2DComponentConstructed(entt::registry& registry, entt::entity entityHandle)
    {
        if(m_IsPlaying)
        {
            Entity entity(entityHandle, this);
            Shared<PhysicsBody2D> physicsBody = Physics2D::CreatePhysicsBody(entity);
            physicsBody->AttachColliders();
        }
    }

    void Scene::OnRigidbody2DComponentDestroyed(entt::registry& registry, entt::entity entityHandle)
    {
        if(m_IsPlaying)
        {
            Entity entity(entityHandle, this);
            Physics2D::DestroyPhysicsBody(entity);
        }
    }

    void Scene::OnBoxCollider2DComponentConstructed(entt::registry& registry, entt::entity entityHandle)
    {
        if(m_IsPlaying)
        {
            Entity entity(entityHandle, this);
            Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

            if(physicsBody)
                physicsBody->AddCollider<BoxCollider2DComponent>(entity);
        }
    }
    void Scene::OnBoxCollider2DComponentDestroyed(entt::registry& registry, entt::entity entityHandle)
    {
        if(m_IsPlaying)
        {
            Entity entity(entityHandle, this);
            Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
            auto& bcComponent = entity.GetComponent<BoxCollider2DComponent>();

            if(physicsBody)
                physicsBody->RemoveCollider(bcComponent.ColliderIndex);
        }
    }

    void Scene::OnCircleCollider2DComponentConstructed(entt::registry& registry, entt::entity entityHandle)
    {
        if(m_IsPlaying)
        {
            Entity entity(entityHandle, this);
            Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

            if(physicsBody)
                physicsBody->AddCollider<CircleCollider2DComponent>(entity);
        }
    }
    void Scene::OnCircleCollider2DComponentDestroyed(entt::registry& registry, entt::entity entityHandle)
    {
        if(m_IsPlaying)
        {
            Entity entity(entityHandle, this);
            Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
            auto& ccComponent = entity.GetComponent<CircleCollider2DComponent>();

            if(physicsBody)
                physicsBody->RemoveCollider(ccComponent.ColliderIndex);
        }
    }

    void Scene::OnCapsuleCollider2DComponentConstructed(entt::registry& registry, entt::entity entityHandle)
    {
        if(m_IsPlaying)
        {
            Entity entity(entityHandle, this);
            Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

            if(physicsBody)
                physicsBody->AddCollider<CapsuleCollider2DComponent>(entity);
        }
    }
    void Scene::OnCapsuleCollider2DComponentDestroyed(entt::registry& registry, entt::entity entityHandle)
    {
        if(m_IsPlaying)
        {
            Entity entity(entityHandle, this);
            Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
            auto& ccComponent = entity.GetComponent<CapsuleCollider2DComponent>();

            if(physicsBody)
                physicsBody->RemoveCollider(ccComponent.ColliderIndex);
        }
    }
}
