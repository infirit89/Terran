#include "trpch.h"
#include "WorldRayCastCallback.h"

#include "PhysicsUtils.h"

namespace TerranEngine 
{
    TerranEngine::WorldRayCastCallback::WorldRayCastCallback(uint16_t layerMask)
        : m_LayerMask(layerMask), m_Fixture(nullptr), m_Point(0.0f, 0.0f), m_Normal(0.0f, 0.0f), m_Fraction(0.0f), m_Hit(false)
    {
    }

    float TerranEngine::WorldRayCastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
    {
        const b2Filter& filterData = fixture->GetFilterData();
        
        if((m_LayerMask & filterData.categoryBits) != 0)
            return fraction == 0.0f ? 0.0f : -1.0f;

        m_Fixture = fixture;
        m_Point = { point.x, point.y };
        m_Normal = { normal.x, normal.y };
        m_Fraction = fraction;
        m_Hit = true;

        return m_Fraction;
    }

    Entity TerranEngine::WorldRayCastCallback::GetHitEntity() const
    {
        if (m_Fixture != nullptr) 
        {
            b2FixtureUserData userData = m_Fixture->GetUserData();

            Entity entity = PhysicsUtils::GetEntityFromB2DUserData(userData);
            return entity;
        }

        return {};
    }
}

