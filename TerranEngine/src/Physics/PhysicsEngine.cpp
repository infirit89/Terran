#include "trpch.h"
#include "PhysicsEngine.h"

#include "Scene/SceneManager.h"

#include <box2d/box2d.h>

namespace TerranEngine 
{
	b2World* PhysicsEngine::s_PhysicsWorld = nullptr;
	
	void PhysicsEngine::CreatePhysicsWorld(const glm::vec2& gravity)
	{
		if (s_PhysicsWorld)
		{
			TR_ERROR("The existing physics world must be deleted before a new one is created");
			return;
		}

		b2Vec2 b2Gravity = { gravity.x, gravity.y };
		s_PhysicsWorld = new b2World(b2Gravity);
	}

	void PhysicsEngine::CleanUpPhysicsWorld()
	{
		if (!s_PhysicsWorld) 
		{
			TR_ERROR("Physics world is null");
			return;
		}

		delete s_PhysicsWorld;
		s_PhysicsWorld = nullptr;
	}

	static b2BodyType ConvertToB2DBodyType(RigidbodyBodyType bodyType) 
	{
		switch (bodyType)
		{
		case TerranEngine::RigidbodyBodyType::Static:		return b2BodyType::b2_staticBody;
		case TerranEngine::RigidbodyBodyType::Dynamic:		return b2BodyType::b2_dynamicBody;
		case TerranEngine::RigidbodyBodyType::Kinematic:	return b2BodyType::b2_kinematicBody;
		}

		TR_ASSERT(false, "Unsupported body type");

		return b2BodyType::b2_staticBody;
	}

	void PhysicsEngine::CreateRigidbody(Entity entity)
	{
		auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();

		b2BodyDef bodyDef;
		bodyDef.type = ConvertToB2DBodyType(rigidbody.BodyType);
		bodyDef.fixedRotation = rigidbody.FixedRotation;
		bodyDef.position.Set(entity.GetTransform().Position.x, entity.GetTransform().Position.y);

		bodyDef.angle = entity.GetTransform().Rotation.z;

		b2Body* body = s_PhysicsWorld->CreateBody(&bodyDef);

		rigidbody.RuntimeBody = body;
	}

	void PhysicsEngine::CreateBoxCollider(Entity entity)
	{
		auto& rididbody = entity.GetComponent<Rigidbody2DComponent>();
		auto& boxCollider = entity.GetComponent<BoxCollider2DComponent>();

		b2PolygonShape shape;
		shape.SetAsBox(entity.GetTransform().Scale.x * boxCollider.Size.x, entity.GetTransform().Scale.y * boxCollider.Size.y);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.5f;

		((b2Body*)rididbody.RuntimeBody)->CreateFixture(&fixtureDef);
	}

	void PhysicsEngine::Update(Time time)
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
			
			b2Body* body = (b2Body*)rigidbody.RuntimeBody;

			transform.Position.x = body->GetPosition().x;
			transform.Position.y = body->GetPosition().y;

			transform.Rotation.z = body->GetAngle();

			transform.IsDirty = true;
		}
	}
}

