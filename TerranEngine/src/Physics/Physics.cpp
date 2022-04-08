#include "trpch.h"
#include "Physics.h"
#include "ContatctListener.h"

#include "Scene/SceneManager.h"

#include <box2d/box2d.h>

#include <glm/gtx/transform.hpp>

namespace TerranEngine 
{
	b2World* Physics2D::s_PhysicsWorld = nullptr;
	static ContactListener s_Listener;
	std::unordered_map<UUID, PhysicsBody2D> Physics2D::s_PhysicsBodies;
	
	static PhysicsBody2D s_EmptyPhysicsBody;

	void Physics2D::CreatePhysicsWorld(const glm::vec2& gravity)
	{
		if (s_PhysicsWorld)
		{
			TR_ERROR("The existing physics world must be deleted before a new one is created");
			return;
		}

		b2Vec2 b2Gravity = { gravity.x, gravity.y };
		s_PhysicsWorld = new b2World(b2Gravity);

		s_PhysicsWorld->SetContactListener(&s_Listener);
	}

	void Physics2D::CleanUpPhysicsWorld()
	{
		if (!s_PhysicsWorld) 
		{
			TR_ERROR("Physics world is null");
			return;
		}

		delete s_PhysicsWorld;
		s_PhysicsWorld = nullptr;

		s_PhysicsBodies.clear();
	}

	void Physics2D::CreatePhysicsBody(Entity entity)
	{
		auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();
		auto& transform = entity.GetTransform();

		b2BodyDef bodyDef;

		bodyDef.position = { transform.Position.x, transform.Position.y };
		bodyDef.angle = transform.Rotation.z;
		bodyDef.fixedRotation = rigidbody.FixedRotation;
		bodyDef.gravityScale = rigidbody.GravityScale;

		b2Body* body = s_PhysicsWorld->CreateBody(&bodyDef);
		PhysicsBody2D physicsBody(body);

		physicsBody.SetBodyState(rigidbody.BodyType);
		physicsBody.SetSleepState(rigidbody.SleepState);

		UUID id = entity.GetID();
		if (entity.HasComponent<BoxCollider2DComponent>()) 
		{
			auto& boxColliderComponent = entity.GetComponent<BoxCollider2DComponent>();

			BoxCollider2D boxCollider;
			boxCollider.SetOffset(boxColliderComponent.Offset);
			boxCollider.SetSize(boxColliderComponent.Size);
			boxCollider.SetSensor(boxColliderComponent.IsSensor);

			boxCollider.SetUserData((void*)id.GetRaw());

			physicsBody.AddBoxCollider(boxCollider);
		}

		if (entity.HasComponent<CircleCollider2DComponent>()) 
		{
			auto& circleColliderComponent = entity.GetComponent<CircleCollider2DComponent>();

			CircleCollider2D circleCollider;

			circleCollider.SetOffset(circleColliderComponent.Offset);
			circleCollider.SetRadius(circleColliderComponent.Radius);
			circleCollider.SetSensor(circleColliderComponent.IsSensor);

			circleCollider.SetUserData((void*)id.GetRaw());

			physicsBody.AddCircleCollider(circleCollider);
		}

		s_PhysicsBodies.emplace(id, std::move(physicsBody));
	}

	void Physics2D::DestroyPhysicsBody(Entity entity)
	{
		if (s_PhysicsWorld) 
		{
			UUID id = entity.GetID();
			if (s_PhysicsBodies.find(id) != s_PhysicsBodies.end()) 
			{
				PhysicsBody2D& physicsBody = s_PhysicsBodies.at(id);

				b2Body* bodyInternal = physicsBody.GetPhysicsBodyInternal();

				if (bodyInternal) 
				{
					s_PhysicsWorld->DestroyBody(bodyInternal);
					physicsBody.SetPhysicsBodyInternal(nullptr);
					s_PhysicsBodies.erase(id);
				}
			}
		}
	}

	void Physics2D::Update(Time time)
	{
		const int32 velocityIterations = 6;
		const int32 positionIterations = 2;

		s_PhysicsWorld->Step(time.GetDeltaTime(), velocityIterations, positionIterations);

		auto rigidbodyView = SceneManager::GetCurrentScene()->GetEntitiesWith<Rigidbody2DComponent>();

		for (auto e : rigidbodyView)
		{
			Entity entity(e, SceneManager::GetCurrentScene().get());

			auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();
			auto& transform = entity.GetTransform();

			PhysicsBody2D& physicsBody = s_PhysicsBodies.at(entity.GetID());

			transform.Position.x = physicsBody.GetPosition().x;
			transform.Position.y = physicsBody.GetPosition().y;

			transform.Rotation.z = physicsBody.GetRotation();

			transform.IsDirty = true;
		}
	}

	PhysicsBody2D& Physics2D::GetPhysicsBody(Entity entity)
	{
		if (s_PhysicsBodies.find(entity.GetID()) != s_PhysicsBodies.end()) 
			return s_PhysicsBodies.at(entity.GetID());

		return s_EmptyPhysicsBody;
	}
}

