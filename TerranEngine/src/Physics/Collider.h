#pragma once

#include <glm/glm.hpp>

namespace TerranEngine 
{
	class Collider2D
	{
	public:
		Collider2D();
		virtual ~Collider2D();

		void SetSensor(bool isSensor) { m_IsSensor = isSensor; }
		void SetUserData(uintptr_t ptr) { m_UserData = ptr; }
		void SetFriction(float friction) { m_Friction = friction; }
		void SetDensity(float density) { m_Density = density; }
		void SetRestitution(float restitution) { m_Restitution = restitution; }
		void SetRestitutionThreshold(float restitutionThreshold) { m_RestitutionThreshold = restitutionThreshold; }
		void SetOffset(const glm::vec2& offset) { m_Offset = offset; };

		bool IsSensor() const { return m_IsSensor; }
		float GetFriction() const { return m_Friction; }
		float GetDensity() const { return m_Density; }
		float GetRestitution() const { return m_Restitution; }
		float GetRestitutionThreshold() const { return m_RestitutionThreshold; }
		uintptr_t GetUserData() const { return m_UserData; }

		inline const glm::vec2& GetOffset() const { return m_Offset; }
		
	private:
		glm::vec2 m_Offset;
		bool m_IsSensor = false;
		uintptr_t m_UserData;
		float m_Friction = 0.5f;
		float m_Density = 1.0f;
		float m_Restitution = 0.0f;
		float m_RestitutionThreshold = 0.0f;
	};

	class BoxCollider2D : public Collider2D
	{
	public:
		BoxCollider2D();
		~BoxCollider2D() override;

		void SetSize(const glm::vec2& size) { m_Size = size; }
		inline const glm::vec2& GetSize() const { return m_Size; }

	private:
		glm::vec2 m_Size;
	};

	class CircleCollider2D : public Collider2D 
	{
	public:
		CircleCollider2D();
		~CircleCollider2D() override;

		void SetRadius(float radius) { m_Radius = radius; }
		inline float GetRadius() const { return m_Radius; }

	private:
		float m_Radius;
	};
}