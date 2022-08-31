#include "trpch.h"

#include "Physics.h"
#include "ContatctListener.h"
#include "WorldRayCastCallback.h"
#include "LayerManager.h"

#include "Core/Settings.h"

#include "Scene/SceneManager.h"

#include "Scripting/ScriptEngine.h"

#include <Physics/PhysicsBody.h>
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
        PhysicsSettings Settings;
	};
	

	static PhysicsEngineState* s_State;
	
	void Physics2D::Initialize()
	{        
		s_State = new PhysicsEngineState();
        s_State->Settings = {  };
        PhysicsLayerManager::SetLayerName(0, "Default");
        PhysicsLayerManager::SetLayerName(1, "IgnoreRayCast");
        PhysicsLayerManager::SetLayerName(3, "Test");
	}

	void Physics2D::Shutdown()
	{
		delete s_State;
	}

	void Physics2D::CreatePhysicsWorld(const PhysicsSettings& settings)
	{
        s_State->Settings = settings;
		if (s_State->PhysicsWorld)
		{
			TR_ERROR("The existing physics world must be deleted before a new one is created");
			return;
		}

		b2Vec2 b2Gravity = { s_State->Settings.Gravity.x, s_State->Settings.Gravity.y };
		s_State->PhysicsWorld = new b2World(b2Gravity);

		s_State->PhysicsWorld->SetContactListener(&s_State->ContactListener);
	}

	void Physics2D::CleanUpPhysicsWorld()
	{
		if (!s_State->PhysicsWorld) 
		{
			TR_ERROR("Physics world is null");
			return;
		}

        s_State->PhysicsBodies.clear();

		delete s_State->PhysicsWorld;
		s_State->PhysicsWorld = nullptr;
	}

    void Physics2D::CratePhysicsBodies(Scene* scene)
    {
        auto rigidbodyView = scene->GetEntitiesWith<Rigidbody2DComponent>();

        for (auto e: rigidbodyView)
        {
            Entity entity(e, scene);
            Shared<PhysicsBody2D> physicsBody = Physics2D::CreatePhysicsBody(entity);
            physicsBody->AttachColliders(entity);
        }
    }

	b2World* Physics2D::GetPhysicsWorld() { return s_State->PhysicsWorld; }

	Shared<PhysicsBody2D> Physics2D::CreatePhysicsBody(Entity entity)
	{
		Shared<PhysicsBody2D> physicsBody = CreateShared<PhysicsBody2D>(entity);
		s_State->PhysicsBodies.emplace(entity.GetID(), physicsBody);
		return physicsBody;
	}

	void Physics2D::DestroyPhysicsBody(Entity entity)
	{
		if (s_State->PhysicsWorld) 
		{
			UUID id = entity.GetID();
			if (s_State->PhysicsBodies.find(id) != s_State->PhysicsBodies.end()) 
			{
				auto& physicsBody = s_State->PhysicsBodies.at(id);

				if (physicsBody) 
					s_State->PhysicsBodies.erase(id);
			}
		}
	}

	void Physics2D::Update(Time time)
	{		
		s_State->PhysicsWorld->Step(s_State->Settings.PhysicsTimestep, s_State->Settings.VelocityIterations, s_State->Settings.PositionIterations);

		s_State->PhysicsDeltaTime += time.GetDeltaTime();

		const auto scriptView = SceneManager::GetCurrentScene()->GetEntitiesWith<ScriptComponent>();
		
		while (s_State->PhysicsDeltaTime > 0.0f)
		{
			for (auto e : scriptView)
			{
				Entity entity(e, SceneManager::GetCurrentScene()->GetRaw());
				ScriptEngine::OnPhysicsUpdate(entity);
			}

			s_State->PhysicsDeltaTime -= s_State->Settings.PhysicsTimestep;
		}

		auto rigidbodyView = SceneManager::GetCurrentScene()->GetEntitiesWith<Rigidbody2DComponent>();

		for (auto e : rigidbodyView)
		{
			Entity entity(e, SceneManager::GetCurrentScene()->GetRaw());

			auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();
			auto& transform = entity.GetTransform();

			Shared<PhysicsBody2D>& physicsBody = s_State->PhysicsBodies.at(entity.GetID());

			transform.Position.x = physicsBody->GetPosition().x;
			transform.Position.y = physicsBody->GetPosition().y;
			transform.Rotation.z = physicsBody->GetRotation();
			transform.IsDirty = true;
		}
	}

	Shared<PhysicsBody2D> Physics2D::GetPhysicsBody(Entity entity)
	{
		if (entity && s_State->PhysicsBodies.find(entity.GetID()) != s_State->PhysicsBodies.end())
			return s_State->PhysicsBodies.at(entity.GetID());

		//return s_State->EmptyPhysicsBody;
        return nullptr;
	}

	bool Physics2D::RayCast(const glm::vec2& origin, const glm::vec2& direction, float length, RayCastHitInfo2D& hitInfo, uint16_t layerMask)
	{
		WorldRayCastCallback raycastCallback(layerMask);
		const b2Vec2 point1 = { origin.x, origin.y };
		const b2Vec2 distance = { length * direction.x, length * direction.y };
		const b2Vec2 point2 = point1 + distance;

		s_State->PhysicsWorld->RayCast(&raycastCallback, point1, point2);

		hitInfo.Normal = raycastCallback.GetNormal();
		hitInfo.Point = raycastCallback.GetPoint();
		hitInfo.PhysicsBody = GetPhysicsBody(raycastCallback.GetHitEntity());

		return raycastCallback.HasHit();
	}
}
