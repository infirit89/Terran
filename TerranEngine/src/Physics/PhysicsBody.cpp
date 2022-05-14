#include "trpch.h"
#include "PhysicsBody.h"
#include "PhysicsUtils.h"
#include "Collider.h"

#include <glm/gtx/transform.hpp>

#include <box2d/box2d.h>
#include <box2d/b2_body.h>

namespace TerranEngine 
{
    PhysicsBody2D::PhysicsBody2D(b2Body* physicsBody)
        : m_Body(physicsBody), m_Entity(PhysicsUtils::GetEntityFromB2DBodyUserData(m_Body->GetUserData()))
    { }


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

    void PhysicsBody2D::AddCollider(BoxCollider2DComponent& colliderComponent, Entity entity)
    {
        // NOTE: think about moving this into box collider creation
        TR_ASSERT(m_Body, "Physics Body is null");

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

        b2Fixture* fixture =  m_Body->CreateFixture(&fixtureDef);

        Shared<BoxCollider2D> boxCollider = CreateShared<BoxCollider2D>(fixture);
        m_Colliders.push_back(boxCollider);
        colliderComponent.ColliderIndex = m_Colliders.size() - 1;
    }

    void PhysicsBody2D::AddCollider(CircleCollider2DComponent& colliderComponent, Entity entity)
    {
        TR_ASSERT(m_Body, "Physics Body is null");

        // NOTE: think about moving this into circle collider creation
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

        b2Fixture* fixture = m_Body->CreateFixture(&fixtureDef);
        Shared<CircleCollider2D> circleCollider = CreateShared<CircleCollider2D>(fixture);
        m_Colliders.push_back(circleCollider);
        colliderComponent.ColliderIndex = m_Colliders.size() - 1;
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
