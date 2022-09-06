#pragma once

#include "Scene/Entity.h"
#include <glm/glm.hpp>

class b2Fixture;
class b2PolygonShape;
class b2CircleShape;
struct b2FixtureDef;

namespace TerranEngine 
{
    class PhysicsBody2D;
	enum class ColliderType2D 
	{
		None = 0,
		Box,
		Circle,
		Edge,
		Chain,
        Capsule
	};

	class Collider2D
	{
	public:
		Collider2D() = default;
        Collider2D(ColliderType2D colliderType, size_t fixtureArraySize);
		Collider2D(ColliderType2D colliderType);
		virtual ~Collider2D();

		void SetSensor(bool isSensor);
		void SetFriction(float friction);
		void SetDensity(float density);
		void SetRestitution(float restitution);
		void SetRestitutionThreshold(float restitutionThreshold);
		virtual void SetOffset(const glm::vec2& offset) = 0;

		bool IsSensor() const;
		float GetFriction() const;
		float GetDensity() const;
		float GetRestitution() const;
		float GetRestitutionThreshold() const;
        uintptr_t GetUserData() const;

		virtual glm::vec2 GetOffset() const = 0;
				
		Shared<PhysicsBody2D> GetPhysicsBody() const;

		inline ColliderType2D GetType() const { return p_ColliderType; }

        void CreateFixture();
        void DestroyFixture();

	protected:
		b2Fixture** p_Fixtures = nullptr;
        b2FixtureDef* p_FixtureDefs = nullptr;
        size_t p_FixtureArraySize;
		ColliderType2D p_ColliderType;

        friend class PhysicsBody2D;
	};

	class BoxCollider2D : public Collider2D
	{
	public:
		BoxCollider2D() = default;
		BoxCollider2D(Entity entity);
		~BoxCollider2D() override;

		void SetSize(const glm::vec2& size);
		glm::vec2 GetSize() const;

		virtual void SetOffset(const glm::vec2& offset) override;
		virtual glm::vec2 GetOffset() const override;

	private:
		b2PolygonShape* m_PolygonShape = nullptr;
	};

	class CircleCollider2D : public Collider2D 
	{
	public:
		CircleCollider2D() = default;
		CircleCollider2D(Entity entity);
		~CircleCollider2D() override;

		void SetRadius(float radius);
		float GetRadius() const;

		virtual void SetOffset(const glm::vec2& offset) override;
		virtual glm::vec2 GetOffset() const override;

	private:
		b2CircleShape* m_CircleShape = nullptr;
	};

    class CapsuleCollider2D : public Collider2D
    {
    public:
        CapsuleCollider2D() = default;
        CapsuleCollider2D(Entity entity);
        ~CapsuleCollider2D() override;
        
		void SetSize(const glm::vec2& size);
		glm::vec2 GetSize() const;

		virtual void SetOffset(const glm::vec2& offset) override;
		virtual glm::vec2 GetOffset() const override;

    private:
        b2CircleShape* m_UpperCircleShape = nullptr, * m_LowerCircleShape = nullptr;
        b2PolygonShape* m_BoxShape = nullptr;
    };
}
