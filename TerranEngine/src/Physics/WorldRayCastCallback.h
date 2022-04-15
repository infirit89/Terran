#pragma once

#include "Scene/Entity.h"

#include <box2d/b2_fixture.h>
#include <box2d/b2_world_callbacks.h>

#include <glm/glm.hpp>

namespace TerranEngine 
{
	class WorldRayCastCallback : public b2RayCastCallback
	{
	public:
		WorldRayCastCallback();

		virtual float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;

		Entity GetHitEntityUUID() const;
		bool HasHit() const { return m_Hit; }
		glm::vec2 GetPoint() const { return m_Point; }
		glm::vec2 GetNormal() const { return m_Normal; }

	private:
		b2Fixture* m_Fixture;
		glm::vec2 m_Point;
		glm::vec2 m_Normal;
		float m_Fraction;
		bool m_Hit;
	};
}