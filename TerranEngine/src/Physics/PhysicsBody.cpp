#include "trpch.h"
#include "PhysicsBody.h"

#include <glm/gtx/transform.hpp>

#include <box2d/box2d.h>
#include <box2d/b2_body.h>

namespace TerranEngine 
{
    PhysicsBody2D::PhysicsBody2D(b2Body* physicsBody)
        : m_Body(physicsBody)
    {
    }


    bool PhysicsBody2D::GetFixedRotation() const
    {
        if (!m_Body)
            return false;

        return m_Body->IsFixedRotation();
    }

    void PhysicsBody2D::SetFixedRotation(bool fixedRotation)
    {
        TR_ASSERT(m_Body, "Physics Body is null");

        m_Body->SetFixedRotation(fixedRotation);
    }

    glm::vec2 PhysicsBody2D::GetPosition() const
    {
        if (!m_Body)
            return { 0.0f, 0.0f };

        b2Vec2 position = m_Body->GetPosition();
        return { position.x, position.y };
    }

    void PhysicsBody2D::SetPosition(const glm::vec2& position)
    {
        TR_ASSERT(m_Body, "Physics Body is null");
        float angle = m_Body->GetAngle();
        m_Body->SetTransform({ position.x, position.y }, angle);
    }

    float PhysicsBody2D::GetRotation() const
    {
        if (!m_Body)
            return 0.0f;

        return m_Body->GetAngle();
    }

    void PhysicsBody2D::SetRotation(float rotation)
    {
        TR_ASSERT(m_Body, "Physics Body is null");
        b2Vec2 position = m_Body->GetPosition();
        m_Body->SetTransform(position, rotation);
    }

    glm::vec2 PhysicsBody2D::GetLinearVelocity() const
    {
        if (!m_Body)
            return { 0.0f, 0.0f };

        b2Vec2 linearVelocity = m_Body->GetLinearVelocity();
        return { linearVelocity.x, linearVelocity.y };
    }

    void PhysicsBody2D::SetLinearVelocity(const glm::vec2& linearVelocity)
    {
        TR_ASSERT(m_Body, "Physics Body is null");
        m_Body->SetLinearVelocity({ linearVelocity.x, linearVelocity.y });
    }

    float PhysicsBody2D::GetAngularVelocity() const
    {
        if (!m_Body)
            return 0.0f;

        return m_Body->GetAngularVelocity();
    }

    void PhysicsBody2D::SetAngularVelocity(float angularVelocity)
    {
        TR_ASSERT(m_Body, "Physics Body is null");

        m_Body->SetAngularVelocity(angularVelocity);
    }

    float PhysicsBody2D::GetGravityScale() const
    {
        if (!m_Body)
            return 1.0f;

        return m_Body->GetGravityScale();
    }

    void PhysicsBody2D::SetGravityScale(float gravityScale)
    {
        TR_ASSERT(m_Body, "Physics Body is null");

        m_Body->SetGravityScale(gravityScale);
    }

    static b2BodyType ConvertToB2DBodyType(PhysicsBodyType bodyType)
    {
        switch (bodyType)
        {
        case TerranEngine::PhysicsBodyType::Static:		return b2BodyType::b2_staticBody;
        case TerranEngine::PhysicsBodyType::Dynamic:	return b2BodyType::b2_dynamicBody;
        case TerranEngine::PhysicsBodyType::Kinematic:	return b2BodyType::b2_kinematicBody;
        }

        TR_ASSERT(false, "Unsupported body type");

        return b2BodyType::b2_dynamicBody;
    }

    static PhysicsBodyType ConvertToTerranPhysicsBodyType(b2BodyType bodyType) 
    {
        switch (bodyType)
        {
        case b2_staticBody:     return PhysicsBodyType::Static;
        case b2_dynamicBody:    return PhysicsBodyType::Dynamic;
        case b2_kinematicBody:  return PhysicsBodyType::Kinematic;
        }

        TR_ASSERT(false, "Unsupported body type");

        return PhysicsBodyType::Dynamic;
    }

    void PhysicsBody2D::SetBodyType(PhysicsBodyType bodyState)
    {
        TR_ASSERT(m_Body, "Physics Body is null");

        m_BodyState = bodyState;
        m_Body->SetType(ConvertToB2DBodyType(m_BodyState));
    }

    void PhysicsBody2D::SetSleepState(PhysicsBodySleepState sleepState)
    {
        TR_ASSERT(m_Body, "Physics Body is null");

        switch (sleepState)
        {
        case TerranEngine::PhysicsBodySleepState::Sleep:
        case TerranEngine::PhysicsBodySleepState::Awake:
            m_Body->SetAwake((bool)sleepState);
            m_Body->SetSleepingAllowed(true);
            break;
        case TerranEngine::PhysicsBodySleepState::NeverSleep:
            m_Body->SetSleepingAllowed(false);
            break;
        default:
            break;
        }
    }

    void PhysicsBody2D::ApplyForce(const glm::vec2& force, const glm::vec2& point, ForceMode2D forceMode)
    {
        TR_ASSERT(m_Body, "Physics Body is null");

        switch (forceMode)
        {
        case TerranEngine::ForceMode2D::Force:
            m_Body->ApplyForce({ force.x, force.y }, { point.x, point.y }, true);
            break;
        case TerranEngine::ForceMode2D::Impulse:
            m_Body->ApplyLinearImpulse({ force.x, force.y }, { point.x, point.y }, true);
            break;
        }
    }

    void PhysicsBody2D::ApplyForceAtCenter(const glm::vec2& force, ForceMode2D forceMode)
    {
        TR_ASSERT(m_Body, "Physics Body is null");

        switch (forceMode)
        {
        case TerranEngine::ForceMode2D::Force:
            m_Body->ApplyForceToCenter({ force.x, force.y }, true);
            break;
        case TerranEngine::ForceMode2D::Impulse:
            m_Body->ApplyLinearImpulseToCenter({ force.x, force.y }, true);
            break;
        }
    }

    void PhysicsBody2D::AddBoxCollider(const BoxCollider2D& boxCollider) 
    {
        TR_ASSERT(m_Body, "Physics Body is null");

        b2PolygonShape shape;

        glm::mat4 transformMat = glm::translate(glm::mat4(1.0f), { boxCollider.GetOffset(), 0.0f }) * glm::scale(glm::mat4(1.0f), { boxCollider.GetSize(), 1.0f });

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
        fixtureDef.density = boxCollider.GetDensity();
        fixtureDef.friction = boxCollider.GetFriction();
        fixtureDef.userData.pointer = boxCollider.GetUserData();
        fixtureDef.isSensor = boxCollider.IsSensor();

        m_Body->CreateFixture(&fixtureDef);
    }

    void PhysicsBody2D::AddCircleCollider(const CircleCollider2D& circleCollider) 
    {
        TR_ASSERT(m_Body, "Physics Body is null");

        b2CircleShape shape;
        shape.m_p.Set(circleCollider.GetOffset().x, circleCollider.GetOffset().y);
        shape.m_radius = circleCollider.GetRadius();

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = circleCollider.GetDensity();
        fixtureDef.friction = circleCollider.GetFriction();
        fixtureDef.userData.pointer = circleCollider.GetUserData();
        fixtureDef.isSensor = circleCollider.IsSensor();

        m_Body->CreateFixture(&fixtureDef);
    }

    PhysicsBodySleepState PhysicsBody2D::GetSleepState() const
    {
        if (!m_Body)
            return PhysicsBodySleepState::Awake;

        if (!m_Body->IsSleepingAllowed())
            return PhysicsBodySleepState::NeverSleep;

        return (PhysicsBodySleepState)m_Body->IsAwake();
    }

    PhysicsBodyType PhysicsBody2D::GetBodyType() const 
    {
        if (!m_Body)
            return PhysicsBodyType::Dynamic;

        return ConvertToTerranPhysicsBodyType(m_Body->GetType());
    }

}
