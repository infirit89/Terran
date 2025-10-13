#pragma once

#include "LibCore/Base.h"

#include "Collider.h"
#include "PhysicsStates.h"

#include "Scene/Entity.h"

#include <Scene/Components.h>
#include <glm/glm.hpp>

#include <concepts>
#include <vector>

class b2Body;

namespace TerranEngine {

class PhysicsBody2D final {
public:
    PhysicsBody2D() = default;
    PhysicsBody2D(Entity entity);
    PhysicsBody2D(b2Body* body);
    ~PhysicsBody2D();

    Entity GetEntity() const { return m_Entity; }

    bool GetFixedRotation() const;
    void SetFixedRotation(bool fixedRotation);

    glm::vec2 GetPosition() const;
    void SetPosition(glm::vec2 const& position);

    float GetRotation() const;
    void SetRotation(float rotation);

    glm::vec2 GetLinearVelocity() const;
    void SetLinearVelocity(glm::vec2 const& linearVelocity);

    float GetAngularVelocity() const;
    void SetAngularVelocity(float angularVelocity);

    float GetGravityScale() const;
    void SetGravityScale(float gravityScale);

    bool IsStatic() const { return m_BodyState == PhysicsBodyType::Static; }
    bool IsDynamic() const { return m_BodyState == PhysicsBodyType::Dynamic; }
    bool IsKinematic() const { return m_BodyState == PhysicsBodyType::Kinematic; }

    void SetBodyType(PhysicsBodyType bodyType);
    PhysicsBodyType GetBodyType() const;

    bool IsAwake() const { return m_SleepState == PhysicsBodySleepState::Awake; }
    bool IsSleeping() const { return m_SleepState == PhysicsBodySleepState::Sleep; }
    bool CanSleep() const { return m_SleepState != PhysicsBodySleepState::NeverSleep; }

    void SetSleepState(PhysicsBodySleepState sleepState);
    PhysicsBodySleepState GetSleepState() const;

    void ApplyForce(glm::vec2 const& force, glm::vec2 const& point, ForceMode2D forceMode);
    void ApplyForceAtCenter(glm::vec2 const& force, ForceMode2D forceMode);

    // NOTE: make a templated function?
    template<typename T>
    void AddCollider(Entity entity)
    {
        TR_ASSERT(m_Body, "Physics Body is null");

        Terran::Core::Shared<Collider2D> collider;
        T& colliderComponent = entity.GetComponent<T>();

        if constexpr (std::same_as<T, CircleCollider2DComponent>)
            collider = Terran::Core::CreateShared<CircleCollider2D>(entity);
        else if constexpr (std::same_as<T, BoxCollider2DComponent>)
            collider = Terran::Core::CreateShared<BoxCollider2D>(entity);
        else if constexpr (std::same_as<T, CapsuleCollider2DComponent>)
            collider = Terran::Core::CreateShared<CapsuleCollider2D>(entity);

        m_Colliders.push_back(collider);
        colliderComponent.ColliderIndex = static_cast<uint32_t>(m_Colliders.size() - 1);
    }

    void RemoveCollider(int index);

    b2Body* GetB2Body() const { return m_Body; }

    operator bool() const { return m_Body != nullptr; }

    std::vector<Terran::Core::Shared<Collider2D>>& GetColliders() { return m_Colliders; }
    Terran::Core::Shared<Collider2D> GetCollider(int index) { return m_Colliders.at(index); }

    void AttachColliders();

private:
    void CreateColliders(Entity entity);

    b2Body* m_Body = nullptr;
    PhysicsBodyType m_BodyState = PhysicsBodyType::Dynamic;
    PhysicsBodySleepState m_SleepState = PhysicsBodySleepState::Awake;
    Entity m_Entity = {};
    std::vector<Terran::Core::Shared<Collider2D>> m_Colliders;
};

}
