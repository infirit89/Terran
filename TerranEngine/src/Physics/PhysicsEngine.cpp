#include "trpch.h"
#include "PhysicsEngine.h"
#include "ContatctListener.h"

#include "Scene/SceneManager.h"

#include <box2d/box2d.h>

#include <glm/gtx/transform.hpp>

namespace TerranEngine 
{
	b2World* PhysicsEngine::s_PhysicsWorld = nullptr;
	static ContactListener s_Listener;

	void PhysicsEngine::CreatePhysicsWorld(const glm::vec2& gravity)
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

		bodyDef.gravityScale = rigidbody.GravityScale;
		bodyDef.awake = (bool)rigidbody.AwakeState;

		if (rigidbody.AwakeState == RigidbodyAwakeState::NeverSleep) 
			bodyDef.allowSleep = false;

		b2Body* body = s_PhysicsWorld->CreateBody(&bodyDef);
		rigidbody.RuntimeBody = body;
	}

	void PhysicsEngine::DestroyRigidbody(Entity entity)
	{
		auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();
		b2Body* body = (b2Body*)rigidbody.RuntimeBody;
		s_PhysicsWorld->DestroyBody(body);
		rigidbody.RuntimeBody = nullptr;
	}

	void PhysicsEngine::CreateBoxCollider(Entity entity)
	{
		auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();
		auto& boxCollider = entity.GetComponent<BoxCollider2DComponent>();

		b2PolygonShape shape;
		//shape.SetAsBox(entity.GetTransform().Scale.x * boxCollider.Size.x * 0.5f, entity.GetTransform().Scale.y * boxCollider.Size.y * 0.5f);

		const glm::vec3 size = { entity.GetTransform().Scale.x * boxCollider.Size.x, entity.GetTransform().Scale.y * boxCollider.Size.y, 1.0f };
		const glm::vec3 position = { boxCollider.Offset.x, boxCollider.Offset.y, 0.0f };

		glm::mat4 transformMat = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), size);

		glm::vec4 positions[4] = {
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f }
		};

		b2Vec2 vertices[4];

		for (size_t i = 0; i < 4; i++) 
		{
			glm::vec3 vertexPos = transformMat * positions[i];
			vertices[i] = { vertexPos.x, vertexPos.y };
		}
		
		shape.Set(vertices, 4);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;

		// TODO: temporary constants for now; create a physics material
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.5f;

		fixtureDef.isSensor = boxCollider.IsSensor;

		const UUID& id = entity.GetID();

		fixtureDef.userData.pointer = (uintptr_t)id.GetRaw();

		((b2Body*)rigidbody.RuntimeBody)->CreateFixture(&fixtureDef);
	}

	void PhysicsEngine::CreateCircleCollider(Entity entity)
	{
		auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();
		auto& circleCollider = entity.GetComponent<CircleCollider2DComponent>();

		b2CircleShape shape;

		shape.m_p.Set(circleCollider.Offset.x, circleCollider.Offset.y);
		shape.m_radius = entity.GetTransform().Scale.x * circleCollider.Radius * 0.5f;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;

		// TODO: temporary constants for now; create a physics material
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.5f;

		fixtureDef.isSensor = circleCollider.IsSensor;

		const UUID& id = entity.GetID();

		fixtureDef.userData.pointer = (uintptr_t)id.GetRaw();

		((b2Body*)rigidbody.RuntimeBody)->CreateFixture(&fixtureDef);
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

