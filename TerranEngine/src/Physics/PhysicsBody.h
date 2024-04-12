#pragma once

#include "Core/Base.h"

#include "PhysicsStates.h"
#include "Collider.h"

#include "Scene/Entity.h"

#include <Scene/Components.h>
#include <glm/glm.hpp>

#include <vector>

class b2Body;

namespace TerranEngine 
{
	class PhysicsBody2D
	{
	public:
		PhysicsBody2D() = default;
		PhysicsBody2D(Entity entity);
		PhysicsBody2D(b2Body* body);
		~PhysicsBody2D();

		Entity GetEntity() const { return m_Entity; }

		bool GetFixedRotation() const;
		void SetFixedRotation(bool fixedRotation);

		glm::vec2 GetPosition() const;
		void SetPosition(const glm::vec2& position);

		float GetRotation() const;
		void SetRotation(float rotation);

		glm::vec2 GetLinearVelocity() const;
		void SetLinearVelocity(const glm::vec2& linearVelocity);

		float GetAngularVelocity() const;
		void SetAngularVelocity(float angularVelocity);

		float GetGravityScale() const;
		void SetGravityScale(float gravityScale);

		inline bool IsStatic() const { return m_BodyState == PhysicsBodyType::Static; }
		inline bool IsDynamic() const { return m_BodyState == PhysicsBodyType::Dynamic; }
		inline bool IsKinematic() const { return m_BodyState == PhysicsBodyType::Kinematic; }

		void SetBodyType(PhysicsBodyType bodyType);
		PhysicsBodyType GetBodyType() const;

		inline bool IsAwake() const { return m_SleepState == PhysicsBodySleepState::Awake; }
		inline bool IsSleeping() const { return m_SleepState == PhysicsBodySleepState::Sleep; }
		inline bool CanSleep() const { return m_SleepState != PhysicsBodySleepState::NeverSleep; }

		void SetSleepState(PhysicsBodySleepState sleepState);
		PhysicsBodySleepState GetSleepState() const;

		void ApplyForce(const glm::vec2& force, const glm::vec2& point, ForceMode2D forceMode);
		void ApplyForceAtCenter(const glm::vec2& force, ForceMode2D forceMode);

		// NOTE: make a templated function?
        template<typename T>
        void AddCollider(Entity entity)
        {
            TR_ASSERT(m_Body, "Physics Body is null");

            Shared<Collider2D> collider; 
            T& colliderComponent = entity.GetComponent<T>();

            if constexpr(std::is_same<T, CircleCollider2DComponent>::value)
                collider = CreateShared<CircleCollider2D>(entity);
            else if constexpr(std::is_same<T, BoxCollider2DComponent>::value)
                collider = CreateShared<BoxCollider2D>(entity);
            else if constexpr(std::is_same<T, CapsuleCollider2DComponent>::value)
                collider = CreateShared<CapsuleCollider2D>(entity);

            m_Colliders.push_back(collider);
            colliderComponent.ColliderIndex = static_cast<uint32_t>(m_Colliders.size() - 1);
        }

        void RemoveCollider(int index);

		b2Body* GetB2Body() const { return m_Body; }
		
		inline operator bool() const { return m_Body != nullptr; }

		inline std::vector<Shared<Collider2D>>& GetColliders() { return m_Colliders; }
		Shared<Collider2D> GetCollider(int index) { return m_Colliders.at(index); }

        void AttachColliders();

	private:
        void CreateColliders(Entity entity);

		b2Body* m_Body = nullptr;
		PhysicsBodyType m_BodyState = PhysicsBodyType::Dynamic;
		PhysicsBodySleepState m_SleepState = PhysicsBodySleepState::Awake;
		Entity m_Entity = {};
		std::vector<Shared<Collider2D>> m_Colliders;
	};
}
