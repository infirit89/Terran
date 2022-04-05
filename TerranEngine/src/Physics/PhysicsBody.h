#pragma once

#include <glm/glm.hpp>

class b2Body;
class b2World;

namespace TerranEngine 
{
	enum class PhysicsBody2DState 
	{
		Static = 0,
		Dynamic,
		Kinematic
	};

	enum class PhysicsBody2DSleepState 
	{
		Sleep = 0,
		Awake,
		NeverSleep
	};

	enum class ForceMode2D
	{
		Force = 0,
		Impulse
	};

	class PhysicsBody2D
	{
	public:
		PhysicsBody2D() = default;
		PhysicsBody2D(b2World* PhysicsWorld, const glm::vec2& position, float rotation);
		~PhysicsBody2D();

		bool GetFixedRotation() const;
		void SetFixedRotation(bool fixedRotation);

		const glm::vec2& GetPosition() const;
		void SetPosition(const glm::vec2& position);

		float GetRotation() const;
		void SetRotation(float rotation);

		const glm::vec2& GetLinearVelocity() const;
		void SetLinearVelocity(const glm::vec2& linearVelocity);

		float GetAngularVelocity() const;
		void SetAngularVelocity(float angularVelocity);

		float GetGravityScale() const;
		void SetGravityScale(float gravityScale);

		inline bool IsStatic() const { return m_BodyState == PhysicsBody2DState::Static; }
		inline bool IsDynamic() const { return m_BodyState == PhysicsBody2DState::Dynamic; }
		inline bool IsKinematic() const { return m_BodyState == PhysicsBody2DState::Kinematic; }

		void SetBodyState(PhysicsBody2DState bodyState);

		inline bool IsAwake() const { return m_SleepState == PhysicsBody2DSleepState::Awake; }
		inline bool IsSleeping() const { return m_SleepState == PhysicsBody2DSleepState::Sleep; }
		inline bool CanSleep() const { return m_SleepState != PhysicsBody2DSleepState::NeverSleep; }

		void SetSleepState(PhysicsBody2DSleepState sleepState);

		void ApplyForce(const glm::vec2& force, const glm::vec2& point, ForceMode2D forceMode);
		void ApplyForceAtCenter(const glm::vec2& force, ForceMode2D forceMode);

	private:
		b2Body* m_Body;
		b2World* m_PhysicsWorld;
		PhysicsBody2DState m_BodyState = PhysicsBody2DState::Dynamic;
		PhysicsBody2DSleepState m_SleepState = PhysicsBody2DSleepState::Awake;
	};
}
