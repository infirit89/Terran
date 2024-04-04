#pragma once

#include "Scene/Entity.h"
#include "PhysicsLayerManager.h"
#include "Physics.h"

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

		/*Entity GetHitEntity() const;*/
		bool HasHit() const { return m_Hit; }
		/*glm::vec2 GetPoint() const { return m_Point; }
		glm::vec2 GetNormal() const { return m_Normal; }*/
		RayCastHitInfo2D GetHitInfo() { return m_HitInfo; }


	private:
        uint16_t m_LayerMask;
		/*b2Fixture* m_Fixture;
		glm::vec2 m_Point;
		glm::vec2 m_Normal;*/
		RayCastHitInfo2D m_HitInfo;
		float m_Fraction;
		bool m_Hit;
	};

	class RayCastMultipleCallback : public b2RayCastCallback 
	{
	public:
		RayCastMultipleCallback(uint16_t layerMask);

		virtual float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;

		size_t GetHitCount() const { return m_HitInfos->size(); }
		//Entity GetHitEntity(int index) const;
		//glm::vec2 GetPoint(int index) const;
		//glm::vec2 GetNormal(int index) const;
		RayCastHitInfo2D GetHitInfo(int index) const { return m_HitInfos->at(index); }
		Shared<std::vector<RayCastHitInfo2D>> GetHitInfos() const { return m_HitInfos; }

	private:
		uint16_t m_LayerMask;

		Shared<std::vector<RayCastHitInfo2D>> m_HitInfos;
	};
}

