#include "trpch.h"

#include "Collider.h"
#include "Physics.h"
#include "PhysicsUtils.h"
#include "LayerManager.h"

#include <glm/gtx/transform.hpp>

#include <box2d/box2d.h>

namespace TerranEngine 
{
	static ColliderType2D GetColliderTypeFromB2DShapeType(b2Shape::Type shapeType) 
	{
		switch (shapeType)
		{
		case b2Shape::e_circle:		return ColliderType2D::Circle;
		case b2Shape::e_edge:		return ColliderType2D::Edge;
		// NOTE: there can also be polygon collider; have to think of a better solution for collider types
		case b2Shape::e_polygon:	return ColliderType2D::Box;	
		case b2Shape::e_chain:		return ColliderType2D::Chain;
		}

		TR_ASSERT(false, "Unknown collider type");

		return ColliderType2D::Box;
	}

	Collider2D::Collider2D(ColliderType2D colliderType)
		: p_ColliderType(colliderType)
	{ }

	void Collider2D::SetSensor(bool isSensor)
	{
		if (!p_Fixture)
			return;

		p_Fixture->SetSensor(isSensor);
	}

	void Collider2D::SetFriction(float friction)
	{
		if (!p_Fixture)
			return;
		
		p_Fixture->SetFriction(friction);
	}

	void Collider2D::SetDensity(float density)
	{
		if (!p_Fixture)
			return;

		p_Fixture->SetDensity(density);
	}

	void Collider2D::SetRestitution(float restitution)
	{
		if (!p_Fixture)
			return;

		p_Fixture->SetRestitution(restitution);
	}

	void Collider2D::SetRestitutionThreshold(float restitutionThreshold)
	{
		if (!p_Fixture)
			return;

		p_Fixture->SetRestitutionThreshold(restitutionThreshold);
	}

	bool Collider2D::IsSensor() const
	{
		TR_ASSERT(p_Fixture, "Fixture is null");
		return p_Fixture->IsSensor();
	}

	float Collider2D::GetFriction() const
	{
		TR_ASSERT(p_Fixture, "Fixture is null");
		return p_Fixture->GetFriction();
	}

	float Collider2D::GetDensity() const
	{
		TR_ASSERT(p_Fixture, "Fixture is null");
		
		return p_Fixture->GetDensity();
	}

	float Collider2D::GetRestitution() const
	{
		TR_ASSERT(p_Fixture, "Fixture is null");
		
		return p_Fixture->GetRestitution();
	}

	float Collider2D::GetRestitutionThreshold() const
	{
		TR_ASSERT(p_Fixture, "Fixture is null");
		
		return p_Fixture->GetRestitutionThreshold();
	}

	uintptr_t Collider2D::GetUserData() const
	{
		TR_ASSERT(p_Fixture, "Fixture is null");
		
		return p_Fixture->GetUserData().pointer;
	}

	Shared<PhysicsBody2D> Collider2D::GetPhysicsBody() const
	{
		Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_Fixture->GetUserData());
		return Physics2D::GetPhysicsBody(entity);
	}

	BoxCollider2D::BoxCollider2D(Entity entity) 
		: Collider2D(ColliderType2D::Box)
	{
		BoxCollider2DComponent& colliderComponent = entity.GetComponent<BoxCollider2DComponent>();
        auto& rigidbodyComponent = entity.GetComponent<Rigidbody2DComponent>();

		b2FixtureDef fixtureDef;

		b2PolygonShape polyhonShape;
		TransformComponent& transform = entity.GetTransform();
		glm::vec2 colliderSize = { transform.Scale.x * colliderComponent.Size.x, transform.Scale.y * colliderComponent.Size.y };

		glm::mat4 transformMat = glm::translate(glm::mat4(1.0f), { colliderComponent.Offset, 0.0f }) *
			glm::scale(glm::mat4(1.0f), { colliderSize, 1.0f });

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

		polyhonShape.Set(vertices, 4);
		fixtureDef.shape = &polyhonShape;

		// NOTE: these magic numbers are temporary until i make physics materials a thing
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.5f;

		const UUID& id = entity.GetID();

		fixtureDef.userData.pointer = (uintptr_t)id.GetRaw();
		fixtureDef.isSensor = colliderComponent.IsSensor;

        fixtureDef.filter.categoryBits =  1 << rigidbodyComponent.LayerIndex;
        PhysicsLayer physicsLayer = PhysicsLayerManager::GetLayer(rigidbodyComponent.LayerIndex);
        fixtureDef.filter.maskBits = physicsLayer.Mask;

		Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

		p_Fixture = physicsBody->GetPhysicsBody()->CreateFixture(&fixtureDef);

		b2Shape* shape = p_Fixture->GetShape();
		m_PolygonShape = dynamic_cast<b2PolygonShape*>(shape);
	}
	
	BoxCollider2D::~BoxCollider2D() { }
	
	void BoxCollider2D::SetSize(const glm::vec2& size)
	{
		TR_ASSERT(m_PolygonShape, "Shape is null");
		glm::vec2 offset = GetOffset();

		m_PolygonShape->SetAsBox(size.x * 0.5f, size.y * 0.5f, { offset.x, offset.y }, 0.0f);
	}
	
	glm::vec2 BoxCollider2D::GetSize() const
	{
		TR_ASSERT(m_PolygonShape, "Shape is null");

		b2Vec2* vertices = m_PolygonShape->m_vertices;

		float sizeX = vertices[0].x + vertices[1].x;
		float sizeY = vertices[1].y + vertices[2].y;

		return { sizeX, sizeY };
	}
	
	void BoxCollider2D::SetOffset(const glm::vec2& offset)
	{
		TR_ASSERT(m_PolygonShape, "Shape is null");

		glm::vec2 size = GetSize();
		m_PolygonShape->SetAsBox(size.x * 0.5f, size.y * 0.5f, { offset.x, offset.y }, 0.0f);
	}
	
	glm::vec2 BoxCollider2D::GetOffset() const
	{
		TR_ASSERT(m_PolygonShape, "Shape is null");

		b2Vec2* vertices = m_PolygonShape->m_vertices;

		float offsetX = (vertices[1].x + vertices[3].x) * 0.5f;
		float offsetY = (vertices[1].y + vertices[3].y) * 0.5f;

		return { offsetX, offsetY };
	}
	
	CircleCollider2D::CircleCollider2D(Entity entity) 
		: Collider2D(ColliderType2D::Circle)
	{
		CircleCollider2DComponent& colliderComponent = entity.GetComponent<CircleCollider2DComponent>();
        auto& rigidbodyComponent = entity.GetComponent<Rigidbody2DComponent>();

		b2FixtureDef fixtureDef;
		b2CircleShape circleShape;

		auto& transform = entity.GetTransform();

		circleShape.m_p.Set(colliderComponent.Offset.x, colliderComponent.Offset.y);
		circleShape.m_radius = ((transform.Scale.x + transform.Scale.y) * 0.5f) * colliderComponent.Radius;

		fixtureDef.shape = &circleShape;

		const UUID& id = entity.GetID();

		// NOTE: these magic numbers are temporary until i make physics materials a thing
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.5f;

		fixtureDef.userData.pointer = (uintptr_t)id.GetRaw();
		fixtureDef.isSensor = colliderComponent.IsSensor;

        fixtureDef.filter.categoryBits =  1 << rigidbodyComponent.LayerIndex;
        PhysicsLayer physicsLayer = PhysicsLayerManager::GetLayer(rigidbodyComponent.LayerIndex);
        fixtureDef.filter.maskBits = physicsLayer.Mask;

		Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
		p_Fixture = physicsBody->GetPhysicsBody()->CreateFixture(&fixtureDef);

		b2Shape* shape = p_Fixture->GetShape();
		m_CircleShape = dynamic_cast<b2CircleShape*>(shape);
	}
	
	CircleCollider2D::~CircleCollider2D() { }
	
	void CircleCollider2D::SetRadius(float radius)
	{
		TR_ASSERT(m_CircleShape, "Shape is null");
		m_CircleShape->m_radius = radius;
	}
	
	float CircleCollider2D::GetRadius() const
	{
		TR_ASSERT(m_CircleShape, "Shape is null");
		return m_CircleShape->m_radius;
	}
	
	void CircleCollider2D::SetOffset(const glm::vec2& offset)
	{
		TR_ASSERT(m_CircleShape, "Shape is null");
		m_CircleShape->m_p.Set(offset.x, offset.y);
	}
	
	glm::vec2 CircleCollider2D::GetOffset() const
	{
		TR_ASSERT(m_CircleShape, "Shape is null");
		return { m_CircleShape->m_p.x, m_CircleShape->m_p.y };
	}
}

