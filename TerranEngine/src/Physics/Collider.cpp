#include "trpch.h"

#include "Collider.h"
#include "Physics.h"
#include "PhysicsBody.h"
#include "PhysicsUtils.h"
#include "PhysicsLayerManager.h"

#include "Core/Assert.h"
#include "Core/Log.h"

#include "Scene/Components.h"
#include "Scene/Entity.h"

#include "Math/Math.h"

#include <float.h>

#include <glm/gtx/transform.hpp>

#include <box2d/box2d.h>

namespace TerranEngine 
{
	/******************************
	 * Base Values:               *
	 * Density = 1.0              *
	 * Friction = 0.5             *
	 * Restitution = 0.0          *
	 * RestitutionThreshold = 0.5 *
	 ******************************/

	// **** Base Collider ****
	Collider2D::Collider2D(ColliderType2D colliderType, size_t fixtureArraySize)
		: p_FixtureArraySize(fixtureArraySize), p_ColliderType(colliderType)
	{
		p_Fixtures = new b2Fixture*[p_FixtureArraySize]();
		p_FixtureDefs = new b2FixtureDef[p_FixtureArraySize]();
	}

	Collider2D::Collider2D(ColliderType2D colliderType)
		: Collider2D::Collider2D(colliderType, 1)
	{ }

	Collider2D::~Collider2D()
	{
		DestroyFixture();

		delete[] p_Fixtures;
		p_Fixtures = nullptr;

		delete[] p_FixtureDefs;
		p_FixtureDefs = nullptr;
	}

	void Collider2D::SetSensor(bool isSensor)
	{
		if (!p_Fixtures)
			return;

		for(int i = 0; i < p_FixtureArraySize; i++)
		{
			if(p_Fixtures[i])
				p_Fixtures[i]->SetSensor(isSensor);
		}
	}

	void Collider2D::SetFriction(float friction)
	{
		if (!p_Fixtures)
			return;
		
		for(int i = 0; i < p_FixtureArraySize; i++)
		{
			if(p_Fixtures[i])
				p_Fixtures[i]->SetFriction(friction);
		} 
	}

	void Collider2D::SetDensity(float density)
	{
		if (!p_Fixtures)
			return;

		for(int i = 0; i < p_FixtureArraySize; i++)
		{
			if(p_Fixtures[i])
				p_Fixtures[i]->SetDensity(density);
		}
	}

	void Collider2D::SetRestitution(float restitution)
	{
		if (!p_Fixtures)
			return;

		for(int i = 0; i < p_FixtureArraySize; i++)
		{
			if(p_Fixtures[i])
				p_Fixtures[i]->SetRestitution(restitution);
		}
	}

	void Collider2D::SetRestitutionThreshold(float restitutionThreshold)
	{
		if (!p_Fixtures)
			return;

		for(int i = 0; i < p_FixtureArraySize; i++)
		{
			if(p_Fixtures[i])
				p_Fixtures[i]->SetRestitutionThreshold(restitutionThreshold);
		} 
	}

	bool Collider2D::IsSensor() const
	{
		TR_ASSERT(p_Fixtures, "Fixture is null");
		
		for (int i = 0; i < p_FixtureArraySize; i++) 
		{
			if(p_Fixtures[i])
				return p_Fixtures[i]->IsSensor();
		}

		TR_ASSERT(true, "No fixtures exist");
		return false;
	}

	float Collider2D::GetFriction() const
	{
		TR_ASSERT(p_Fixtures, "Fixture is null");
		for (int i = 0; i < p_FixtureArraySize; i++) 
		{
			if(p_Fixtures[i])
				return p_Fixtures[i]->GetFriction();
		}

		TR_ASSERT(true, "No fixtures exist");
		return 0.5f;
	}

	float Collider2D::GetDensity() const
	{
		TR_ASSERT(p_Fixtures, "Fixture is null");
		
		for (int i = 0; i < p_FixtureArraySize; i++) 
		{
			if(p_Fixtures[i])
				return p_Fixtures[i]->GetDensity();
		}

		TR_ASSERT(true, "No fixtures exist");
		return 1.0f;
	}

	float Collider2D::GetRestitution() const
	{
		TR_ASSERT(p_Fixtures, "Fixture is null");
		
		for (int i = 0; i < p_FixtureArraySize; i++) 
		{
			if(p_Fixtures[i])
				return p_Fixtures[i]->GetRestitution();
		}

		TR_ASSERT(true, "No fixtures exist");
		return 0.0f;
	}

	float Collider2D::GetRestitutionThreshold() const
	{
		TR_ASSERT(p_Fixtures, "Fixture is null");
		
		for (int i = 0; i < p_FixtureArraySize; i++) 
		{
			if(p_Fixtures[i])
				return p_Fixtures[i]->GetRestitutionThreshold();
		}        

		TR_ASSERT(true, "No fixtures exist");
		return 0.5f;
	}

	uintptr_t Collider2D::GetUserData() const
	{
		TR_ASSERT(p_Fixtures, "Fixture is null");
		
		for (int i = 0; i < p_FixtureArraySize; i++) 
		{
			if(p_Fixtures[i])
				return p_Fixtures[i]->GetUserData().pointer;
		}

		TR_ASSERT(true, "No fixtures exist");
		return NULL;
	}

	Shared<PhysicsBody2D> Collider2D::GetPhysicsBody() const
	{
		TR_ASSERT(p_Fixtures, "Fixture is null");

		for (int i = 0; i < p_FixtureArraySize; i++) 
		{
			if(p_Fixtures[i])
			{
				Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_Fixtures[i]->GetUserData());
				return Physics2D::GetPhysicsBody(entity);
			}
		}

		TR_ASSERT(true, "No fixtures exist");
		return nullptr;
	}

	void Collider2D::CreateFixture()
	{
		TR_ASSERT(p_FixtureDefs, "The fixture definitions array hasn't been created yet!");
		Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_FixtureDefs->userData);
		TR_ASSERT(entity, "Invalid UUID");
		Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
		
		for(int i = 0; i < p_FixtureArraySize; i++)
		{
			if(!p_FixtureDefs[i].shape)
				continue;

			p_Fixtures[i] = physicsBody->GetB2Body()->CreateFixture(&p_FixtureDefs[i]);
		}

	}

	void Collider2D::DestroyFixture()
	{
		TR_ASSERT(p_Fixtures, "The fixture array hasn't been created yet!");
		for (int i = 0; i < p_FixtureArraySize; i++) 
		{
			if(!p_Fixtures[i])    
				continue;

			p_Fixtures[i]->GetBody()->DestroyFixture(p_Fixtures[i]);
			p_Fixtures[i] = nullptr;
			p_FixtureDefs[i].shape = nullptr;
		}
	}

	// ***********************

	// **** Box Collider ****
	BoxCollider2D::BoxCollider2D(Entity entity) 
		: Collider2D(ColliderType2D::Box)
	{
		m_PolygonShape = new b2PolygonShape();

		BoxCollider2DComponent& colliderComponent = entity.GetComponent<BoxCollider2DComponent>();
		auto& rigidbodyComponent = entity.GetComponent<Rigidbody2DComponent>();

		TransformComponent& transform = entity.GetTransform();

		glm::vec2 colliderSize = { transform.Scale.x * colliderComponent.Size.x, transform.Scale.y * colliderComponent.Size.y };

		// NOTE: these magic numbers are temporary until i make physics materials a thing
		p_FixtureDefs[0].density = 1.0f;
		p_FixtureDefs[0].friction = 0.5f;
		p_FixtureDefs[0].restitution = 0.0f;
		p_FixtureDefs[0].restitutionThreshold = 0.5f;

		const UUID& id = entity.GetID();

		p_FixtureDefs[0].userData.pointer = (uintptr_t)id.GetRaw();
		p_FixtureDefs[0].isSensor = colliderComponent.IsSensor;

		p_FixtureDefs[0].filter.categoryBits =  1 << rigidbodyComponent.LayerIndex;
		PhysicsLayer physicsLayer = PhysicsLayerManager::GetLayer(rigidbodyComponent.LayerIndex);
		p_FixtureDefs[0].filter.maskBits = physicsLayer.Mask;
		
		// box2d cant create colliders with an area less than the float machine epsilon
		if(colliderSize.x * colliderSize.y <= b2_epsilon)
			return;
		
		m_PolygonShape->SetAsBox(colliderSize.x * 0.5f, colliderSize.y * 0.5f, 
								{ colliderComponent.Offset.x, colliderComponent.Offset.y }, 0.0f);

		p_FixtureDefs[0].shape = m_PolygonShape;
	}
	
	BoxCollider2D::~BoxCollider2D()
	{
		delete m_PolygonShape;
		m_PolygonShape = nullptr;
	}
	
	void BoxCollider2D::SetSize(const glm::vec2& size)
	{
		TR_ASSERT(m_PolygonShape, "Shape is null");

		// NOTE: profile the function; may be slow
		Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_FixtureDefs[0].userData);
		TR_ASSERT(entity, "Invalid UUID!");

		auto& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
		auto& transform = entity.GetTransform();

		glm::vec2 offset = bcComponent.Offset;

		if(p_Fixtures[0])
			DestroyFixture();
		
		glm::vec2 colliderSize = { transform.Scale.x * size.x, transform.Scale.y * size.y };
		
		if(colliderSize.x * colliderSize.y <= b2_epsilon)
			return;

		m_PolygonShape->SetAsBox(colliderSize.x * 0.5f, colliderSize.y * 0.5f, { offset.x, offset.y }, 0.0f);
		p_FixtureDefs[0].shape = m_PolygonShape;
		CreateFixture();

		bcComponent.Size = size;
	}
	
	glm::vec2 BoxCollider2D::GetSize() const
	{
		TR_ASSERT(m_PolygonShape, "Shape is null");

		/*if(!p_Fixtures[0])*/
			/*return { 0.0f, 0.0f };*/

		/*b2Vec2* vertices = m_PolygonShape->m_vertices;*/

		/*float sizeX = vertices[0].x + vertices[1].x;*/
		/*float sizeY = vertices[1].y + vertices[2].y;*/

		/*return { sizeX, sizeY };*/

		Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_FixtureDefs[0].userData);
		TR_ASSERT(entity, "Invalid UUID!");

		auto& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
		return bcComponent.Size;
	}
	
	void BoxCollider2D::SetOffset(const glm::vec2& offset)
	{
		TR_ASSERT(m_PolygonShape, "Shape is null");

		// NOTE: profile the function; may be slow
		Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_FixtureDefs[0].userData);
		TR_ASSERT(entity, "Invalid UUID!");

		auto& bcComponent = entity.GetComponent<BoxCollider2DComponent>();

		if(p_Fixtures[0])
			DestroyFixture();

		glm::vec2 size = GetSize();
		m_PolygonShape->SetAsBox(size.x * 0.5f, size.y * 0.5f, { offset.x, offset.y }, 0.0f);
		p_FixtureDefs[0].shape = m_PolygonShape;
		CreateFixture();

		bcComponent.Offset = offset;
	}
	
	glm::vec2 BoxCollider2D::GetOffset() const
	{
		TR_ASSERT(m_PolygonShape, "Shape is null");

		//if(!p_Fixtures[0])
			//return { 0.0f, 0.0f };

		//b2Vec2* vertices = m_PolygonShape->m_vertices;

		//float offsetX = (vertices[1].x + vertices[3].x) * 0.5f;
		//float offsetY = (vertices[1].y + vertices[3].y) * 0.5f;

		//return { offsetX, offsetY };
		Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_FixtureDefs[0].userData);
		TR_ASSERT(entity, "Invalid UUID!");

		auto& bcComponent = entity.GetComponent<BoxCollider2DComponent>();
		return bcComponent.Offset;
	}
	// **********************
	
	// **** Circle Collider ****
	CircleCollider2D::CircleCollider2D(Entity entity) 
		: Collider2D(ColliderType2D::Circle)
	{
		m_CircleShape = new b2CircleShape();

		CircleCollider2DComponent& colliderComponent = entity.GetComponent<CircleCollider2DComponent>();
		auto& rigidbodyComponent = entity.GetComponent<Rigidbody2DComponent>();

		auto& transform = entity.GetTransform();

		glm::vec3 position, rotation, scale;
		Math::Decompose(transform.WorldSpaceTransformMatrix, position, rotation, scale);

		float scalingFactor = scale.x > scale.y ? scale.x : scale.y;
		float radius = scalingFactor * colliderComponent.Radius;
		radius = std::max(0.0f, radius);

		m_CircleShape->m_p.Set(colliderComponent.Offset.x, colliderComponent.Offset.y);
		m_CircleShape->m_radius = radius;

		p_FixtureDefs[0].shape = m_CircleShape;

		const UUID& id = entity.GetID();

		// NOTE: these magic numbers are temporary until i make physics materials a thing
		p_FixtureDefs[0].density = 1.0f;
		p_FixtureDefs[0].friction = 0.5f;
		p_FixtureDefs[0].restitution = 0.0f;
		p_FixtureDefs[0].restitutionThreshold = 0.5f;

		p_FixtureDefs[0].userData.pointer = (uintptr_t)id.GetRaw();
		p_FixtureDefs[0].isSensor = colliderComponent.IsSensor;

		p_FixtureDefs[0].filter.categoryBits =  1 << rigidbodyComponent.LayerIndex;
		PhysicsLayer physicsLayer = PhysicsLayerManager::GetLayer(rigidbodyComponent.LayerIndex);
		p_FixtureDefs[0].filter.maskBits = physicsLayer.Mask;
	}
	
	CircleCollider2D::~CircleCollider2D()
	{
		delete m_CircleShape;
		m_CircleShape = nullptr;
	}
	
	void CircleCollider2D::SetRadius(float radius)
	{
		TR_ASSERT(m_CircleShape, "Shape is null");

		// NOTE: profile the function; may be slow
		Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_FixtureDefs[0].userData);
		TR_ASSERT(entity, "Invalid UUID!");

		auto& ccComponent = entity.GetComponent<CircleCollider2DComponent>();

		DestroyFixture();
		m_CircleShape->m_radius = radius;
		p_FixtureDefs[0].shape = m_CircleShape;
		CreateFixture();

		ccComponent.Radius = radius;
	}
	
	float CircleCollider2D::GetRadius() const
	{
		TR_ASSERT(m_CircleShape, "Shape is null");
		return m_CircleShape->m_radius;
	}
	
	void CircleCollider2D::SetOffset(const glm::vec2& offset)
	{
		TR_ASSERT(m_CircleShape, "Shape is null");

		// NOTE: profile the function; may be slow
		Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_FixtureDefs[0].userData);
		TR_ASSERT(entity, "Invalid UUID!");

		auto& ccComponent = entity.GetComponent<CircleCollider2DComponent>();

		DestroyFixture();
		m_CircleShape->m_p.Set(offset.x, offset.y);
		p_FixtureDefs[0].shape = m_CircleShape;
		CreateFixture();

		ccComponent.Offset = offset;
	}
	
	glm::vec2 CircleCollider2D::GetOffset() const
	{
		TR_ASSERT(m_CircleShape, "Shape is null");
		return { m_CircleShape->m_p.x, m_CircleShape->m_p.y };
	}
	// *************************

	// **** Capsule Collider ****
	CapsuleCollider2D::CapsuleCollider2D(Entity entity)
		: Collider2D(ColliderType2D::Capsule, 3)
	{
		m_UpperCircleShape = new b2CircleShape();
		m_LowerCircleShape = new b2CircleShape();

		m_BoxShape = new b2PolygonShape();

		auto& colliderComponent = entity.GetComponent<CapsuleCollider2DComponent>();
		auto& rigidbodyComponent = entity.GetComponent<Rigidbody2DComponent>();

		const UUID& id = entity.GetID();

		// NOTE: these magic numbers are temporary until i make physics materials a thing
		p_FixtureDefs[0].density = 1.0f;
		p_FixtureDefs[0].friction = 0.5f;
		p_FixtureDefs[0].restitution = 0.0f;
		p_FixtureDefs[0].restitutionThreshold = 0.5f;

		p_FixtureDefs[0].userData.pointer = (uintptr_t)id.GetRaw();
		p_FixtureDefs[0].isSensor = colliderComponent.IsSensor;

		p_FixtureDefs[0].filter.categoryBits =  1 << rigidbodyComponent.LayerIndex;
		PhysicsLayer physicsLayer = PhysicsLayerManager::GetLayer(rigidbodyComponent.LayerIndex);
		p_FixtureDefs[0].filter.maskBits = physicsLayer.Mask;

		p_FixtureDefs[1] = p_FixtureDefs[0];
		p_FixtureDefs[2] = p_FixtureDefs[0];

		auto& transform = entity.GetTransform();

		float circleRadius = colliderComponent.Size.x * 0.5f;

		glm::vec2 boxSize = { transform.Scale.x * colliderComponent.Size.x, 
							  transform.Scale.y * colliderComponent.Size.y - (circleRadius * 2.0f) };

		boxSize = glm::max({ 0.0f, 0.0f }, boxSize);
		
		glm::vec2 upperCirclePosition = { colliderComponent.Offset.x, colliderComponent.Offset.y + (boxSize.y * 0.5f) };
		glm::vec2 lowerCirclePosition = { colliderComponent.Offset.x, colliderComponent.Offset.y - (boxSize.y * 0.5f) };

		// box
		if(boxSize.x * boxSize.y > b2_epsilon)
		{
			m_BoxShape->SetAsBox(boxSize.x * 0.5f, boxSize.y * 0.5f, { colliderComponent.Offset.x, colliderComponent.Offset.y }, 0.0f);
			p_FixtureDefs[0].shape = m_BoxShape;
		}
		
		// upper circle
		{
			m_UpperCircleShape->m_p.Set(upperCirclePosition.x, upperCirclePosition.y);
			m_UpperCircleShape->m_radius = ((transform.Scale.x + transform.Scale.y) * 0.5f) * circleRadius;
			p_FixtureDefs[1].shape = m_UpperCircleShape;
		}

		// lower circle
		{
			m_LowerCircleShape->m_p.Set(lowerCirclePosition.x, lowerCirclePosition.y);
			m_LowerCircleShape->m_radius = ((transform.Scale.x + transform.Scale.y) * 0.5f) * circleRadius;
			p_FixtureDefs[2].shape = m_LowerCircleShape;
		}
	}

	CapsuleCollider2D::~CapsuleCollider2D() 
	{
		delete m_LowerCircleShape;
		m_LowerCircleShape = nullptr;

		delete m_UpperCircleShape;
		m_UpperCircleShape = nullptr;

		delete m_BoxShape;
		m_BoxShape = nullptr;
	}

	// TODO: implement
	void CapsuleCollider2D::SetSize(const glm::vec2& size)
	{
		TR_ASSERT(m_LowerCircleShape && m_UpperCircleShape && m_BoxShape, "Shapes are null");

		// NOTE: profile the function; may be slow
		Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_FixtureDefs[0].userData);
		TR_ASSERT(entity, "Invalid UUID!");

		auto& ccComponent = entity.GetComponent<CapsuleCollider2DComponent>();
		auto& transform = entity.GetTransform();

		if (p_Fixtures[0] || p_Fixtures[1] || p_Fixtures[2])
			DestroyFixture();


		float circleRadius = size.x * 0.5f;

		glm::vec2 boxSize = { transform.Scale.x * size.x,
							  transform.Scale.y * size.y - (circleRadius * 2.0f) };

		boxSize = glm::max({ 0.0f, 0.0f }, boxSize);

		glm::vec2 upperCirclePosition = { ccComponent.Offset.x, ccComponent.Offset.y + (boxSize.y * 0.5f) };
		glm::vec2 lowerCirclePosition = { ccComponent.Offset.x, ccComponent.Offset.y - (boxSize.y * 0.5f) };

		// box shape
		if (boxSize.x * boxSize.y > b2_epsilon)
		{
			m_BoxShape->SetAsBox(boxSize.x * 0.5f, boxSize.y * 0.5f, 
								{ ccComponent.Offset.x, ccComponent.Offset.y }, 0.0f);
			p_FixtureDefs[0].shape = m_BoxShape;
		}

		// upper circle shape
		{
			m_UpperCircleShape->m_p.Set(upperCirclePosition.x, upperCirclePosition.y);
			m_UpperCircleShape->m_radius = ((transform.Scale.x + transform.Scale.y) * 0.5f) * circleRadius;
			p_FixtureDefs[1].shape = m_UpperCircleShape;
		}

		// lower circle shape
		{
			m_LowerCircleShape->m_p.Set(lowerCirclePosition.x, lowerCirclePosition.y);
			m_LowerCircleShape->m_radius = ((transform.Scale.x + transform.Scale.y) * 0.5f) * circleRadius;
			p_FixtureDefs[2].shape = m_LowerCircleShape;
		}

		CreateFixture();

		ccComponent.Size = size;
	}

	glm::vec2 CapsuleCollider2D::GetSize() const
	{
		TR_ASSERT(m_LowerCircleShape && m_UpperCircleShape && m_BoxShape, "Shapes are null");

		Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_FixtureDefs[0].userData);
		TR_ASSERT(entity, "Invalid UUID!");

		auto& ccComponent = entity.GetComponent<CapsuleCollider2DComponent>();
		return ccComponent.Size;
	}

	void CapsuleCollider2D::SetOffset(const glm::vec2& offset)
	{
		TR_ASSERT(m_LowerCircleShape && m_UpperCircleShape && m_BoxShape, "Shapes are null");

		Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_FixtureDefs[0].userData);
		TR_ASSERT(entity, "Invalid UUID!");

		auto& ccComponent = entity.GetComponent<CapsuleCollider2DComponent>();
		auto& transform = entity.GetTransform();

		if (p_Fixtures[0] || p_Fixtures[1] || p_Fixtures[2])
			DestroyFixture();


		float circleRadius = ccComponent.Size.x * 0.5f;

		glm::vec2 boxSize = { transform.Scale.x * ccComponent.Size.x,
							  transform.Scale.y * ccComponent.Size.y - (circleRadius * 2.0f) };

		boxSize = glm::max({ 0.0f, 0.0f }, boxSize);

		glm::vec2 upperCirclePosition = { offset.x, offset.y + (boxSize.y * 0.5f) };
		glm::vec2 lowerCirclePosition = { offset.x, offset.y - (boxSize.y * 0.5f) };

		// box shape
		if (boxSize.x * boxSize.y > b2_epsilon)
		{
			m_BoxShape->SetAsBox(boxSize.x * 0.5f, boxSize.y * 0.5f, 
								{ offset.x, offset.y }, 0.0f);

			p_FixtureDefs[0].shape = m_BoxShape;
		}

		// upper circle shape
		{
			m_UpperCircleShape->m_p.Set(upperCirclePosition.x, upperCirclePosition.y);
			m_UpperCircleShape->m_radius = ((transform.Scale.x + transform.Scale.y) * 0.5f) * circleRadius;
			p_FixtureDefs[1].shape = m_UpperCircleShape;
		}

		// lower circle shape
		{
			m_LowerCircleShape->m_p.Set(lowerCirclePosition.x, lowerCirclePosition.y);
			m_LowerCircleShape->m_radius = ((transform.Scale.x + transform.Scale.y) * 0.5f) * circleRadius;
			p_FixtureDefs[2].shape = m_LowerCircleShape;
		}

		CreateFixture();

		ccComponent.Offset = offset;
	}

	glm::vec2 CapsuleCollider2D::GetOffset() const 
	{
		TR_ASSERT(m_LowerCircleShape && m_UpperCircleShape && m_BoxShape, "Shapes are null");

		Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_FixtureDefs[0].userData);
		TR_ASSERT(entity, "Invalid UUID!");

		auto& ccComponent = entity.GetComponent<CapsuleCollider2DComponent>();
		return ccComponent.Offset;
	}
	// **************************
}

