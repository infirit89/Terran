#include "trpch.h"
#include "Physics.h"
#include "ContatctListener.h"
#include "WorldRayCastCallback.h"

#include "Core/Settings.h"

#include "Scene/SceneManager.h"

#include "Scripting/ScriptEngine.h"

#include <box2d/box2d.h>

#include <glm/gtx/transform.hpp>

namespace TerranEngine 
{
	struct PhysicsEngineState
	{
		b2World* PhysicsWorld = nullptr;
		std::unordered_map<UUID, Shared<PhysicsBody2D>> PhysicsBodies;
		float PhysicsDeltaTime = 0.0f;
		ContactListener ContactListener;
		Shared<PhysicsBody2D> EmptyPhysicsBody;
	};
	

	static PhysicsEngineState* s_PhysicsEngineState;
	
	void Physics2D::Initialize()
	{
		s_PhysicsEngineState = new PhysicsEngineState();
	}

	void Physics2D::Shutdown()
	{
		delete s_PhysicsEngineState;
	}

	void Physics2D::CreatePhysicsWorld(const glm::vec2& gravity)
	{
		if (s_PhysicsEngineState->PhysicsWorld)
		{
			TR_ERROR("The existing physics world must be deleted before a new one is created");
			return;
		}

		b2Vec2 b2Gravity = { gravity.x, gravity.y };
		s_PhysicsEngineState->PhysicsWorld = new b2World(b2Gravity);

		s_PhysicsEngineState->PhysicsWorld->SetContactListener(&s_PhysicsEngineState->ContactListener);
	}

	void Physics2D::CleanUpPhysicsWorld()
	{
		if (!s_PhysicsEngineState->PhysicsWorld) 
		{
			TR_ERROR("Physics world is null");
			return;
		}
        s_PhysicsEngineState->PhysicsBodies.clear();

		delete s_PhysicsEngineState->PhysicsWorld;
		s_PhysicsEngineState->PhysicsWorld = nullptr;
	}

    void Physics2D::CratePhysicsBodies(Scene* scene)
    {
        auto rigidbodyView = scene->GetEntitiesWith<Rigidbody2DComponent>();

        for (auto e: rigidbodyView)
        {
            Entity entity(e, scene);
            Physics2D::CreatePhysicsBody(entity);
        }
    }

	b2World* Physics2D::GetPhysicsWorld() { return s_PhysicsEngineState->PhysicsWorld; }

	void Physics2D::CreatePhysicsBody(Entity entity)
	{
		Shared<PhysicsBody2D> physicsBody = CreateShared<PhysicsBody2D>(entity);
		s_PhysicsEngineState->PhysicsBodies.emplace(entity.GetID(), physicsBody);
        physicsBody->AttachColliders(entity);
	}

	void Physics2D::DestroyPhysicsBody(Entity entity)
	{
		if (s_PhysicsEngineState->PhysicsWorld) 
		{
			UUID id = entity.GetID();
			if (s_PhysicsEngineState->PhysicsBodies.find(id) != s_PhysicsEngineState->PhysicsBodies.end()) 
			{
				auto& physicsBody = s_PhysicsEngineState->PhysicsBodies.at(id);

				if (physicsBody) 
					s_PhysicsEngineState->PhysicsBodies.erase(id);
			}
		}
	}

	void Physics2D::Update(Time time)
	{		
		s_PhysicsEngineState->PhysicsWorld->Step(Settings::PhysicsFixedTimestep, Settings::PhysicsVelocityIterations, Settings::PhysicsPositionIterations);

		s_PhysicsEngineState->PhysicsDeltaTime += time.GetDeltaTime();

		const auto scriptView = SceneManager::GetCurrentScene()->GetEntitiesWith<ScriptComponent>();
		
		while (s_PhysicsEngineState->PhysicsDeltaTime > 0.0f)
		{
			for (auto e : scriptView)
			{
				Entity entity(e, SceneManager::GetCurrentScene()->GetRaw());
				ScriptEngine::OnPhysicsUpdate(entity);
			}

			s_PhysicsEngineState->PhysicsDeltaTime -= Settings::PhysicsFixedTimestep;
		}

		auto rigidbodyView = SceneManager::GetCurrentScene()->GetEntitiesWith<Rigidbody2DComponent>();

		for (auto e : rigidbodyView)
		{
			Entity entity(e, SceneManager::GetCurrentScene()->GetRaw());

			auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();
			auto& transform = entity.GetTransform();

			Shared<PhysicsBody2D>& physicsBody = s_PhysicsEngineState->PhysicsBodies.at(entity.GetID());

			if (transform.Position.x != physicsBody->GetPosition().x || transform.Position.y != physicsBody->GetPosition().y) 
			{
				transform.Position.x = physicsBody->GetPosition().x;
				transform.Position.y = physicsBody->GetPosition().y;
				transform.IsDirty = true;
			}

			if (transform.Rotation.z != physicsBody->GetRotation())
			{
				transform.Rotation.z = physicsBody->GetRotation();
				transform.IsDirty = true;
			}
		}
	}

	Shared<PhysicsBody2D>& Physics2D::GetPhysicsBody(Entity entity)
	{
		if (entity && s_PhysicsEngineState->PhysicsBodies.find(entity.GetID()) != s_PhysicsEngineState->PhysicsBodies.end())
			return s_PhysicsEngineState->PhysicsBodies.at(entity.GetID());

		return s_PhysicsEngineState->EmptyPhysicsBody;
	}

	bool Physics2D::RayCast(const glm::vec2& origin, const glm::vec2& direction, float length, RayCastHitInfo2D& hitInfo)
	{
		WorldRayCastCallback raycastCallback;
		const b2Vec2 point1 = { origin.x, origin.y };
		const b2Vec2 distance = { length * direction.x, length * direction.y };
		const b2Vec2 point2 = point1 + distance;

		s_PhysicsEngineState->PhysicsWorld->RayCast(&raycastCallback, point1, point2);

		hitInfo.Normal = raycastCallback.GetNormal();
		hitInfo.Point = raycastCallback.GetPoint();
		hitInfo.PhysicsBody = GetPhysicsBody(raycastCallback.GetHitEntity());

		return raycastCallback.HasHit();
	}
}
