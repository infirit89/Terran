#include "trpch.h"
#include "Physics.h"
#include "ContatctListener.h"
#include "WorldRayCastCallback.h"

#include "Core/Settings.h"

#include "Scripting/ScriptEngine.h"

#include <box2d/box2d.h>

#include <glm/gtx/transform.hpp>

namespace TerranEngine 
{
	struct PhysicsEngineState
	{
		b2World* PhysicsWorld = nullptr;
		std::unordered_map<UUID, PhysicsBody2D> PhysicsBodies;
		float PhysicsDeltaTime = 0.0f;
		Shared<Scene> SceneContext = nullptr;
		ContactListener ContactListener;
		PhysicsBody2D EmptyPhysicsBody;
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

		delete s_PhysicsEngineState->PhysicsWorld;
		s_PhysicsEngineState->PhysicsWorld = nullptr;

		s_PhysicsEngineState->PhysicsBodies.clear();
	}

	void Physics2D::CreatePhysicsBody(Entity entity)
	{
		auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();
		auto& transform = entity.GetTransform();

		const UUID& id = entity.GetID();

		b2BodyDef bodyDef;

		bodyDef.position = { transform.Position.x, transform.Position.y };
		bodyDef.angle = transform.Rotation.z;
		bodyDef.fixedRotation = rigidbody.FixedRotation;
		bodyDef.gravityScale = rigidbody.GravityScale;
		bodyDef.userData.pointer = (uintptr_t)id.GetRaw();
		bodyDef.enabled = rigidbody.Enabled;

		b2Body* body = s_PhysicsEngineState->PhysicsWorld->CreateBody(&bodyDef);
		PhysicsBody2D physicsBody(body);

		physicsBody.SetBodyType(rigidbody.BodyType);
		physicsBody.SetSleepState(rigidbody.SleepState);

		if (entity.HasComponent<BoxCollider2DComponent>()) 
		{
			auto& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
			physicsBody.AddCollider(bcComponent, entity);
		}

		if (entity.HasComponent<CircleCollider2DComponent>()) 
		{
			auto& ccComponent = entity.GetComponent<CircleCollider2DComponent>();
			physicsBody.AddCollider(ccComponent, entity);
		}

		s_PhysicsEngineState->PhysicsBodies.emplace(id, std::move(physicsBody));
	}

	void Physics2D::DestroyPhysicsBody(Entity entity)
	{
		if (s_PhysicsEngineState->PhysicsWorld) 
		{
			UUID id = entity.GetID();
			if (s_PhysicsEngineState->PhysicsBodies.find(id) != s_PhysicsEngineState->PhysicsBodies.end()) 
			{
				PhysicsBody2D& physicsBody = s_PhysicsEngineState->PhysicsBodies.at(id);

				b2Body* bodyInternal = physicsBody.GetPhysicsBodyInternal();

				if (bodyInternal) 
				{
					s_PhysicsEngineState->PhysicsWorld->DestroyBody(bodyInternal);
					physicsBody.SetPhysicsBodyInternal(nullptr);
					s_PhysicsEngineState->PhysicsBodies.erase(id);
				}
			}
		}
	}

	void Physics2D::Update(Time time)
	{		
		s_PhysicsEngineState->PhysicsWorld->Step(Settings::PhysicsFixedTimestep, Settings::PhysicsVelocityIterations, Settings::PhysicsPositionIterations);

		s_PhysicsEngineState->PhysicsDeltaTime += time.GetDeltaTime();

		const auto scriptView = s_PhysicsEngineState->SceneContext->GetEntitiesWith<ScriptComponent>();
		
		while (s_PhysicsEngineState->PhysicsDeltaTime > 0.0f)
		{
			for (auto e : scriptView)
			{
				Entity entity(e, s_PhysicsEngineState->SceneContext->GetRaw());
				ScriptEngine::OnPhysicsUpdate(entity);
			}

			s_PhysicsEngineState->PhysicsDeltaTime -= Settings::PhysicsFixedTimestep;
		}

		auto rigidbodyView = s_PhysicsEngineState->SceneContext->GetEntitiesWith<Rigidbody2DComponent>();

		for (auto e : rigidbodyView)
		{
			Entity entity(e, s_PhysicsEngineState->SceneContext->GetRaw());

			auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();
			auto& transform = entity.GetTransform();

			PhysicsBody2D& physicsBody = s_PhysicsEngineState->PhysicsBodies.at(entity.GetID());

			if (transform.Position.x != physicsBody.GetPosition().x || transform.Position.y != physicsBody.GetPosition().y) 
			{
				transform.Position.x = physicsBody.GetPosition().x;
				transform.Position.y = physicsBody.GetPosition().y;
				transform.IsDirty = true;
			}

			if (transform.Rotation.z != physicsBody.GetRotation()) 
			{
				transform.Rotation.z = physicsBody.GetRotation();
				transform.IsDirty = true;
			}
		}
	}

	void Physics2D::SetContext(const Shared<Scene>& context) { s_PhysicsEngineState->SceneContext = context; }
	Shared<Scene>& Physics2D::GetContext() { return s_PhysicsEngineState->SceneContext; }

	PhysicsBody2D& Physics2D::GetPhysicsBody(Entity entity)
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
