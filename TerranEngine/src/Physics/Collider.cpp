#include "trpch.h"

#include "Collider.h"
#include "Physics.h"
#include "PhysicsBody.h"
#include "PhysicsUtils.h"
#include "LayerManager.h"

#include <Core/Assert.h>
#include <Core/Log.h>
#include <Scene/Components.h>
#include <Scene/Entity.h>
#include <float.h>
#include <glm/gtx/transform.hpp>

#include <box2d/b2_fixture.h>
#include <box2d/box2d.h>
#include <box2d/b2_common.h>

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
        p_Fixture = new b2Fixture*[p_FixtureArraySize]();
    }

	Collider2D::Collider2D(ColliderType2D colliderType)
		: Collider2D::Collider2D(colliderType, 1)
	{ }

    Collider2D::~Collider2D()
    {
        delete[] p_Fixture;
        p_Fixture = nullptr;
    }

	void Collider2D::SetSensor(bool isSensor)
	{
		if (!p_Fixture)
			return;

        for(int i = 0; i < p_FixtureArraySize; i++)
        {
            if(p_Fixture[i])
                p_Fixture[i]->SetSensor(isSensor);
        }
	}

	void Collider2D::SetFriction(float friction)
	{
		if (!p_Fixture)
			return;
		
        for(int i = 0; i < p_FixtureArraySize; i++)
        {
            if(p_Fixture[i])
                p_Fixture[i]->SetFriction(friction);
        } 
	}

	void Collider2D::SetDensity(float density)
	{
		if (!p_Fixture)
			return;

        for(int i = 0; i < p_FixtureArraySize; i++)
        {
            if(p_Fixture[i])
                p_Fixture[i]->SetDensity(density);
        }
	}

	void Collider2D::SetRestitution(float restitution)
	{
		if (!p_Fixture)
			return;

        for(int i = 0; i < p_FixtureArraySize; i++)
        {
            if(p_Fixture[i])
                p_Fixture[i]->SetRestitution(restitution);
        }
	}

	void Collider2D::SetRestitutionThreshold(float restitutionThreshold)
	{
		if (!p_Fixture)
			return;

        for(int i = 0; i < p_FixtureArraySize; i++)
        {
            if(p_Fixture[i])
                p_Fixture[i]->SetRestitutionThreshold(restitutionThreshold);
        } 
	}

	bool Collider2D::IsSensor() const
	{
		TR_ASSERT(p_Fixture, "Fixture is null");
        
        for (int i = 0; i < p_FixtureArraySize; i++) 
        {
            if(p_Fixture[i])
                return p_Fixture[i]->IsSensor();
        }

        TR_ASSERT(true, "No fixtures exist");
        return false;
	}

	float Collider2D::GetFriction() const
	{
		TR_ASSERT(p_Fixture, "Fixture is null");
        for (int i = 0; i < p_FixtureArraySize; i++) 
        {
            if(p_Fixture[i])
                return p_Fixture[i]->GetFriction();
        }

        TR_ASSERT(true, "No fixtures exist");
        return 0.5f;
	}

	float Collider2D::GetDensity() const
	{
		TR_ASSERT(p_Fixture, "Fixture is null");
		
        for (int i = 0; i < p_FixtureArraySize; i++) 
        {
            if(p_Fixture[i])
                return p_Fixture[i]->GetDensity();
        }

        TR_ASSERT(true, "No fixtures exist");
        return 1.0f;
	}

	float Collider2D::GetRestitution() const
	{
		TR_ASSERT(p_Fixture, "Fixture is null");
		
        for (int i = 0; i < p_FixtureArraySize; i++) 
        {
            if(p_Fixture[i])
                return p_Fixture[i]->GetRestitution();
        }

        TR_ASSERT(true, "No fixtures exist");
        return 0.0f;
	}

	float Collider2D::GetRestitutionThreshold() const
	{
		TR_ASSERT(p_Fixture, "Fixture is null");
		
        for (int i = 0; i < p_FixtureArraySize; i++) 
        {
            if(p_Fixture[i])
                return p_Fixture[i]->GetRestitutionThreshold();
        }        

        TR_ASSERT(true, "No fixtures exist");
        return 0.5f;
	}

	uintptr_t Collider2D::GetUserData() const
	{
		TR_ASSERT(p_Fixture, "Fixture is null");
		
        for (int i = 0; i < p_FixtureArraySize; i++) 
        {
            if(p_Fixture[i])
                return p_Fixture[i]->GetUserData().pointer;
        }

        TR_ASSERT(true, "No fixtures exist");
        return NULL;
	}

	Shared<PhysicsBody2D> Collider2D::GetPhysicsBody() const
	{
		TR_ASSERT(p_Fixture, "Fixture is null");

        for (int i = 0; i < p_FixtureArraySize; i++) 
        {
            if(p_Fixture[i])
            {
                Entity entity = PhysicsUtils::GetEntityFromB2DUserData(p_Fixture[i]->GetUserData());
                return Physics2D::GetPhysicsBody(entity);
            }
        }

        TR_ASSERT(true, "No fixtures exist");
        return nullptr;
	}
    // ***********************

    // **** Box Collider ****
	BoxCollider2D::BoxCollider2D(Entity entity) 
		: Collider2D(ColliderType2D::Box)
	{
		BoxCollider2DComponent& colliderComponent = entity.GetComponent<BoxCollider2DComponent>();
        auto& rigidbodyComponent = entity.GetComponent<Rigidbody2DComponent>();

		b2FixtureDef fixtureDef;

		b2PolygonShape polyhonShape;
		TransformComponent& transform = entity.GetTransform();

		glm::vec2 colliderSize = { transform.Scale.x * colliderComponent.Size.x, transform.Scale.y * colliderComponent.Size.y };

        // box2d cant create colliders with a size of 0 on an axis
        if(colliderSize.x <= b2_epsilon || colliderSize.y <= b2_epsilon)
            return;
        
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
		fixtureDef.restitution = 0.0f;
		fixtureDef.restitutionThreshold = 0.5f;

		const UUID& id = entity.GetID();

		fixtureDef.userData.pointer = (uintptr_t)id.GetRaw();
		fixtureDef.isSensor = colliderComponent.IsSensor;

        fixtureDef.filter.categoryBits =  1 << rigidbodyComponent.LayerIndex;
        PhysicsLayer physicsLayer = PhysicsLayerManager::GetLayer(rigidbodyComponent.LayerIndex);
        fixtureDef.filter.maskBits = physicsLayer.Mask;

		Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);

		p_Fixture[0] = physicsBody->GetPhysicsBody()->CreateFixture(&fixtureDef);

		b2Shape* shape = p_Fixture[0]->GetShape();
		m_PolygonShape = dynamic_cast<b2PolygonShape*>(shape);
        
	}
	
	BoxCollider2D::~BoxCollider2D() { Collider2D::~Collider2D(); }
	
	void BoxCollider2D::SetSize(const glm::vec2& size)
	{
		TR_ASSERT(m_PolygonShape, "Shape is null");
		glm::vec2 offset = GetOffset();
        
        // TODO: if the size is invalid and the user changes it to a valid one the collider should be created
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
    // **********************
	
    // **** Circle Collider ****
	CircleCollider2D::CircleCollider2D(Entity entity) 
		: Collider2D(ColliderType2D::Circle)
	{
		CircleCollider2DComponent& colliderComponent = entity.GetComponent<CircleCollider2DComponent>();
        auto& rigidbodyComponent = entity.GetComponent<Rigidbody2DComponent>();

		auto& transform = entity.GetTransform();
        float radius = ((transform.Scale.x + transform.Scale.y) * 0.5f) * colliderComponent.Radius;
        radius = std::max(0.0f, radius);

        // box2d cant create a cirlcle shape with a radius of 0
        if(radius <= b2_epsilon)
            return;

        b2FixtureDef fixtureDef;
        b2CircleShape circleShape;

		circleShape.m_p.Set(colliderComponent.Offset.x, colliderComponent.Offset.y);
		circleShape.m_radius = radius;

		fixtureDef.shape = &circleShape;

		const UUID& id = entity.GetID();

		// NOTE: these magic numbers are temporary until i make physics materials a thing
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.5f;
        fixtureDef.restitution = 0.0f;
		fixtureDef.restitutionThreshold = 0.5f;

		fixtureDef.userData.pointer = (uintptr_t)id.GetRaw();
		fixtureDef.isSensor = colliderComponent.IsSensor;

        fixtureDef.filter.categoryBits =  1 << rigidbodyComponent.LayerIndex;
        PhysicsLayer physicsLayer = PhysicsLayerManager::GetLayer(rigidbodyComponent.LayerIndex);
        fixtureDef.filter.maskBits = physicsLayer.Mask;

		Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
		p_Fixture[0] = physicsBody->GetPhysicsBody()->CreateFixture(&fixtureDef);

		b2Shape* shape = p_Fixture[0]->GetShape();
		m_CircleShape = dynamic_cast<b2CircleShape*>(shape);
	}
	
	CircleCollider2D::~CircleCollider2D() { Collider2D::~Collider2D(); }
	
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
    // *************************

    // **** Capsule Collider ****
    CapsuleCollider2D::CapsuleCollider2D(Entity entity)
        : Collider2D(ColliderType2D::Capsule, 3)
    {
		auto& colliderComponent = entity.GetComponent<CapsuleCollider2DComponent>();
        auto& rigidbodyComponent = entity.GetComponent<Rigidbody2DComponent>();

        if(colliderComponent.Size.x <= b2_epsilon || colliderComponent.Size.y <= b2_epsilon)
            return;

		b2FixtureDef fixtureDef;

        const UUID& id = entity.GetID();

        // NOTE: these magic numbers are temporary until i make physics materials a thing
        fixtureDef.density = 1.0f;
        fixtureDef.friction = 0.5f;
        fixtureDef.restitution = 0.0f;
		fixtureDef.restitutionThreshold = 0.5f;

        fixtureDef.userData.pointer = (uintptr_t)id.GetRaw();
        fixtureDef.isSensor = colliderComponent.IsSensor;

        fixtureDef.filter.categoryBits =  1 << rigidbodyComponent.LayerIndex;
        PhysicsLayer physicsLayer = PhysicsLayerManager::GetLayer(rigidbodyComponent.LayerIndex);
        fixtureDef.filter.maskBits = physicsLayer.Mask;

		auto& transform = entity.GetTransform();

        float circleRadius = colliderComponent.Size.x * 0.5f;

        glm::vec2 boxSize = { colliderComponent.Size.x, colliderComponent.Size.y - (circleRadius * 2.0f) };
        boxSize = glm::max({ 0.0f, 0.0f }, boxSize);
		
        glm::vec2 upperCirclePosition = { colliderComponent.Offset.x, colliderComponent.Offset.y + (boxSize.y * 0.5f) };
        glm::vec2 lowerCirclePosition = { colliderComponent.Offset.x, colliderComponent.Offset.y - (boxSize.y * 0.5f) };

        // box
        if(boxSize.x > b2_epsilon && boxSize.y > b2_epsilon)
        {
            b2PolygonShape boxShape;
            b2Vec2 boxCenter = { colliderComponent.Offset.x, colliderComponent.Offset.y };
            boxShape.SetAsBox(boxSize.x * 0.5f, boxSize.y * 0.5f, boxCenter, 0.0f);
            
            fixtureDef.shape = &boxShape;

            Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
            p_Fixture[0] = physicsBody->GetPhysicsBody()->CreateFixture(&fixtureDef);

            b2Shape* shape = p_Fixture[0]->GetShape();
            m_BoxShape = dynamic_cast<b2PolygonShape*>(shape);
        }
        
        // upper circle
        {
            b2CircleShape circleShape;
            circleShape.m_p.Set(upperCirclePosition.x, upperCirclePosition.y);
            circleShape.m_radius = ((transform.Scale.x + transform.Scale.y) * 0.5f) * circleRadius;

            fixtureDef.shape = &circleShape;

            Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
            p_Fixture[1] = physicsBody->GetPhysicsBody()->CreateFixture(&fixtureDef);

            b2Shape* shape = p_Fixture[1]->GetShape();
            m_UpperCircleShape = dynamic_cast<b2CircleShape*>(shape);
        }

        // lower circle
        {
            b2CircleShape circleShape;
            circleShape.m_p.Set(lowerCirclePosition.x, lowerCirclePosition.y);
            circleShape.m_radius = ((transform.Scale.x + transform.Scale.y) * 0.5f) * circleRadius;

            fixtureDef.shape = &circleShape;

            Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
            p_Fixture[2] = physicsBody->GetPhysicsBody()->CreateFixture(&fixtureDef);

            b2Shape* shape = p_Fixture[2]->GetShape();
            m_LowerCircleShape = dynamic_cast<b2CircleShape*>(shape);
        }
    }

    CapsuleCollider2D::~CapsuleCollider2D() { Collider2D::~Collider2D(); }

    // TODO: implement
    void CapsuleCollider2D::SetSize(const glm::vec2& size)
    {

    }

    glm::vec2 CapsuleCollider2D::GetSize() const
    {
		return { 0.0f, 0.0f };
    }

    void CapsuleCollider2D::SetOffset(const glm::vec2& offset)
    {

    }

    glm::vec2 CapsuleCollider2D::GetOffset() const 
    {
		return { 0.0f, 0.0f };
    }
    // **************************
}

