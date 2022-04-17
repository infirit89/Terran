#pragma once

#include "Collider.h"
#include "PhysicsStates.h"

#include "Scene/Entity.h"

#include <glm/glm.hpp>

class b2Body;
class b2World;

namespace TerranEngine 
{
	class PhysicsBody2D
	{
	public:
		PhysicsBody2D() = default;
		PhysicsBody2D(b2Body* physicsBody);
		~PhysicsBody2D() = default;

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

		void AddBoxCollider(const BoxCollider2D& collider);
		void AddCircleCollider(const CircleCollider2D& collider);

		b2Body* GetPhysicsBodyInternal() const { return m_Body; }
		void SetPhysicsBodyInternal(b2Body* body) { m_Body = body; }

		inline operator bool() const { return m_Body != nullptr; }

	private:
		b2Body* m_Body = nullptr;
		PhysicsBodyType m_BodyState = PhysicsBodyType::Dynamic;
		PhysicsBodySleepState m_SleepState = PhysicsBodySleepState::Awake;
		Entity m_Entity = {};
	};
}
