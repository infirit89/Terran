#pragma once

#include "Core/Base.h"
#include "PhysicsStates.h"

#include "Scene/Entity.h"

#include <glm/glm.hpp>

#include <vector>

class b2Body;

namespace TerranEngine 
{
	class Collider2D;
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
		void AddBoxCollider(Entity entity);
		void AddCircleCollider(Entity entity);

		b2Body* GetPhysicsBody() const { return m_Body; }
		
		inline operator bool() const { return m_Body != nullptr; }

		inline std::vector<Shared<Collider2D>>& GetColliders() { return m_Colliders; }

	private:
		void AttachColliders(Entity entity);

		b2Body* m_Body = nullptr;
		PhysicsBodyType m_BodyState = PhysicsBodyType::Dynamic;
		PhysicsBodySleepState m_SleepState = PhysicsBodySleepState::Awake;
		Entity m_Entity = {};
		std::vector<Shared<Collider2D>> m_Colliders;
	};
}
