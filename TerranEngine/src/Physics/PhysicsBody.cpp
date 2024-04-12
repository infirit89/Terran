#include "trpch.h"

#include "PhysicsBody.h"
#include "PhysicsUtils.h"
#include "Collider.h"
#include "Physics.h"

#include "Scene/Components.h"

#include "Math/Math.h"

#include <glm/gtx/transform.hpp>

#include <box2d/box2d.h>
#include <box2d/b2_body.h>

namespace TerranEngine 
{
    // TODO: pass the b2World to the contrustor to create a b2Body
    PhysicsBody2D::PhysicsBody2D(Entity entity)
        : m_Entity(entity)
    {
        auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();
        auto& transform = entity.GetTransform();

        const UUID& id = entity.GetID();

        b2BodyDef bodyDef;

        glm::vec3 position = transform.Position;
        glm::vec3 rotation = transform.Rotation;
        glm::vec3 scale;

        if (entity.HasParent()) 
        {
            glm::mat4 worldTransformMatrix = entity.GetTransform().WorldSpaceTransformMatrix;
            Math::Decompose(worldTransformMatrix, position, rotation, scale);
        }

        bodyDef.position = { position.x, position.y };
        bodyDef.angle = rotation.z;
        bodyDef.fixedRotation = rigidbody.FixedRotation;
        bodyDef.gravityScale = rigidbody.GravityScale;
        bodyDef.userData.pointer = (uintptr_t)id.GetRaw();
        bodyDef.enabled = rigidbody.Enabled;
        
        m_Body = Physics2D::GetB2World()->CreateBody(&bodyDef);
        
        SetBodyType(rigidbody.BodyType);
        SetSleepState(rigidbody.SleepState);

        CreateColliders(entity);
    }

    PhysicsBody2D::PhysicsBody2D(b2Body* body) 
        : m_Body(body), 
        m_Entity(PhysicsUtils::GetEntityFromB2DUserData(body->GetUserData()))
    { }

    PhysicsBody2D::~PhysicsBody2D() 
    {
        m_Colliders.clear();
        Physics2D::GetB2World()->DestroyBody(m_Body);
        m_Body = nullptr;
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
        SetSleepState(PhysicsBodySleepState::Awake);
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
        SetSleepState(PhysicsBodySleepState::Awake);
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
        case PhysicsBodyType::Static:       return b2BodyType::b2_staticBody;
        case PhysicsBodyType::Dynamic:      return b2BodyType::b2_dynamicBody;
        case PhysicsBodyType::Kinematic:    return b2BodyType::b2_kinematicBody;
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

    void PhysicsBody2D::RemoveCollider(int index)
    {
        if (!m_Body)
            return;

        Shared<Collider2D> collider = m_Colliders.at(index);
        for(int i = 0; i < collider->p_FixtureArraySize; i++)
            m_Body->DestroyFixture(collider->p_Fixtures[i]);
        m_Colliders.erase(m_Colliders.begin() + index);
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
    
    void PhysicsBody2D::AttachColliders() 
    {
        for (auto& collider : m_Colliders) 
            collider->CreateFixture();
    }

    void PhysicsBody2D::CreateColliders(Entity entity)
    {
        if (entity.HasComponent<BoxCollider2DComponent>())
            AddCollider<BoxCollider2DComponent>(entity);
        
        if (entity.HasComponent<CircleCollider2DComponent>())
            AddCollider<CircleCollider2DComponent>(entity);

        if (entity.HasComponent<CapsuleCollider2DComponent>())
            AddCollider<CapsuleCollider2DComponent>(entity);
    }
}

