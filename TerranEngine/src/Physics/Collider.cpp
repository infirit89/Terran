#include "trpch.h"
#include "Collider.h"

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
	}

	Collider2D::Collider2D(b2Fixture* fixture)
		: p_Fixture(fixture)
	{
		b2Shape* shape = p_Fixture->GetShape();
		m_ColliderType = GetColliderTypeFromB2DShapeType(shape->GetType());
	}

	Collider2D::~Collider2D()
	{
	}

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

	PhysicsBody2D Collider2D::GetPhysicsBody() const
	{
		PhysicsBody2D physicsBody(p_Fixture->GetBody());
		return physicsBody;
	}

	BoxCollider2D::BoxCollider2D(b2Fixture* fixture) : Collider2D(fixture)
	{
		b2Shape* shape = p_Fixture->GetShape();

		if (shape->GetType() == b2Shape::e_polygon)
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
	
	CircleCollider2D::CircleCollider2D(b2Fixture* fixture) : Collider2D(fixture)
	{
		b2Shape* shape = p_Fixture->GetShape();

		if (shape->GetType() == b2Shape::e_circle)
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

