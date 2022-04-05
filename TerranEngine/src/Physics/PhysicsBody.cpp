#include "trpch.h"
#include "PhysicsBody.h"

#include <box2d/box2d.h>
#include <box2d/b2_body.h>

namespace TerranEngine 
{
    PhysicsBody2D::PhysicsBody2D(b2World* PhysicsWorld, const glm::vec2& position, float rotation)
        : m_PhysicsWorld(PhysicsWorld)
    {
        if (m_PhysicsWorld) 
        {
            b2BodyDef bodyDef;

            bodyDef.position = { position.x, position.y };
            bodyDef.angle = rotation;

            m_Body = m_PhysicsWorld->CreateBody(&bodyDef);
        }
    }

    PhysicsBody2D::~PhysicsBody2D()
    {
        if (m_PhysicsWorld && m_Body) 
        {
            m_PhysicsWorld->DestroyBody(m_Body);
            m_Body = nullptr;
        }
    }

    bool PhysicsBody2D::GetFixedRotation() const
    {
        return m_Body->IsFixedRotation();
    }

    void PhysicsBody2D::SetFixedRotation(bool fixedRotation)
    {
        m_Body->SetFixedRotation(fixedRotation);
    }

    const glm::vec2& PhysicsBody2D::GetPosition() const
    {
        b2Vec2 position = m_Body->GetPosition();

        return { position.x, position.y };
    }

    void PhysicsBody2D::SetPosition(const glm::vec2& position)
    {
        float angle = m_Body->GetAngle();

        m_Body->SetTransform({ position.x, position.y }, angle);
    }

    float PhysicsBody2D::GetRotation() const
    {
        return m_Body->GetAngle();
    }

    void PhysicsBody2D::SetRotation(float rotation)
    {
        b2Vec2 position = m_Body->GetPosition();

        m_Body->SetTransform(position, rotation);
    }

    const glm::vec2& PhysicsBody2D::GetLinearVelocity() const
    {
        b2Vec2 linearVelocity = m_Body->GetLinearVelocity();

        return { linearVelocity.x, linearVelocity.y };
    }

    void PhysicsBody2D::SetLinearVelocity(const glm::vec2& linearVelocity)
    {
        m_Body->SetLinearVelocity({ linearVelocity.x, linearVelocity.y });
    }

    float PhysicsBody2D::GetAngularVelocity() const
    {
        return m_Body->GetAngularVelocity();
    }

    void PhysicsBody2D::SetAngularVelocity(float angularVelocity)
    {
        m_Body->SetAngularVelocity(angularVelocity);
    }

    float PhysicsBody2D::GetGravityScale() const
    {
        return m_Body->GetGravityScale();
    }

    void PhysicsBody2D::SetGravityScale(float gravityScale)
    {
        m_Body->SetGravityScale(gravityScale);
    }

    static b2BodyType ConvertToB2DBodyType(PhysicsBody2DState bodyType)
    {
        switch (bodyType)
        {
        case TerranEngine::PhysicsBody2DState::Static:		return b2BodyType::b2_staticBody;
        case TerranEngine::PhysicsBody2DState::Dynamic:		return b2BodyType::b2_dynamicBody;
        case TerranEngine::PhysicsBody2DState::Kinematic:	return b2BodyType::b2_kinematicBody;
        }

        TR_ASSERT(false, "Unsupported body type");

        return b2BodyType::b2_staticBody;
    }

    void PhysicsBody2D::SetBodyState(PhysicsBody2DState bodyState)
    {
        m_BodyState = bodyState;
        m_Body->SetType(ConvertToB2DBodyType(m_BodyState));
    }

    void PhysicsBody2D::SetSleepState(PhysicsBody2DSleepState sleepState)
    {
        switch (sleepState)
        {
        case TerranEngine::PhysicsBody2DSleepState::Sleep:
        case TerranEngine::PhysicsBody2DSleepState::Awake:
            m_Body->SetAwake((bool)sleepState);
            m_Body->SetSleepingAllowed(true);
            break;
        case TerranEngine::PhysicsBody2DSleepState::NeverSleep:
            m_Body->SetSleepingAllowed(false);
            break;
        default:
            break;
        }
    }

    void PhysicsBody2D::ApplyForce(const glm::vec2& force, const glm::vec2& point, ForceMode2D forceMode)
    {
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
}
