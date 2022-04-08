#include "trpch.h"
#include "Collider.h"

#include <box2d/box2d.h>

namespace TerranEngine 
{
	Collider2D::Collider2D()
		: m_Offset(0.0f, 0.0f)
	{
		//b2Fixture* fixture;
		//b2Shape* shape = fixture->GetShape();

		//switch (shape->m_type)
		//{
		//case b2Shape::e_polygon:
		//	b2PolygonShape* pShape = dynamic_cast<b2PolygonShape*>(shape);

		//	if (pShape) 
		//	{
		//		// reconstruct shape;
		//	}
		//}
	}

	Collider2D::~Collider2D()
	{
	}

	BoxCollider2D::BoxCollider2D() 
		: m_Size(0.0f, 0.0f)
	{
		/*b2PolygonShape shape;

		glm::mat4 transformMat = glm::translate(glm::mat4(1.0f), { offset.x, offset.y, 0.0f }) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

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

		p_FixtureDef->shape = &shape;*/
	}

	BoxCollider2D::~BoxCollider2D()
	{
	}

	CircleCollider2D::CircleCollider2D()
		: m_Radius(0.0f)
	{
		/*b2CircleShape shape;
		shape.m_p.Set(offset.x, offset.y);
		shape.m_radius = radius;

		p_FixtureDef->shape = &shape;*/
	}

	CircleCollider2D::~CircleCollider2D()
	{
	}
}

