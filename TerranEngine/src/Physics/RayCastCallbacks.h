#pragma once

#include "Scene/Entity.h"
#include "PhysicsLayerManager.h"

#include <box2d/b2_fixture.h>
#include <box2d/b2_world_callbacks.h>

#include <glm/glm.hpp>

#include <vector>

namespace TerranEngine 
{
	class RayCastClosestCallback : public b2RayCastCallback
	{
	public:
		RayCastClosestCallback(uint16_t layerMask);

		virtual float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;

		Entity GetHitEntity() const;
		bool HasHit() const { return m_Hit; }
		glm::vec2 GetPoint() const { return m_Point; }
		glm::vec2 GetNormal() const { return m_Normal; }

	private:
        uint16_t m_LayerMask;
		b2Fixture* m_Fixture;
		glm::vec2 m_Point;
		glm::vec2 m_Normal;
		float m_Fraction;
		bool m_Hit;
	};

	class RayCastMultipleCallback : public b2RayCastCallback 
	{
	public:
		RayCastMultipleCallback(uint16_t layerMask, glm::vec2 origin, float length);

		virtual float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;

		size_t GetHitCount() { return m_HitInfos.size(); }
		
	private:
		uint16_t m_LayerMask;

		struct HitInfo_Internal
		{
			b2Fixture* Fixture;
			glm::vec2 Point;
			glm::vec2 Normal;
			float Fraction;

			bool operator<(const HitInfo_Internal& other) 
			{
				return glm::all(glm::lessThan(Normal, other.Normal));
			}
		};

		std::vector<HitInfo_Internal> m_HitInfos;
		bool m_Hit;
		glm::vec2 m_Origin;
		float m_Length;
	};
}

