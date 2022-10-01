#include "trpch.h"
#include "RayCastCallbacks.h"

#include "PhysicsUtils.h"

namespace TerranEngine 
{
    RayCastClosestCallback::RayCastClosestCallback(uint16_t layerMask)
        : m_LayerMask(layerMask), m_HitInfo(), /*m_Fixture(nullptr), m_Point(0.0f, 0.0f), m_Normal(0.0f, 0.0f),*/ m_Fraction(0.0f), m_Hit(false)
    { }
    
    float RayCastClosestCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
    {
        const b2Filter& filterData = fixture->GetFilterData();
        
        if((m_LayerMask & filterData.maskBits) == 0)
            return fraction == 0.0f ? 0.0f : -1.0f;

        /*m_Fixture = fixture;
        m_Point = { point.x, point.y };
        m_Normal = { normal.x, normal.y };*/
        m_HitInfo.Normal = { normal.x, normal.y };
        m_HitInfo.Point = { point.x, point.y };

        Entity entity = {};
        if (fixture != nullptr)
        {
            b2FixtureUserData userData = fixture->GetUserData();
            entity = PhysicsUtils::GetEntityFromB2DUserData(userData);
        }
        m_HitInfo.PhysicsBody = Physics2D::GetPhysicsBody(entity);

        m_Fraction = fraction;

        m_Hit = true;

        return m_Fraction;
    }

    /*Entity RayCastClosestCallback::GetHitEntity() const
    {
        if (m_Fixture != nullptr) 
        {
            b2FixtureUserData userData = m_Fixture->GetUserData();

            Entity entity = PhysicsUtils::GetEntityFromB2DUserData(userData);
            return entity;
        }

        return {};
    }*/


    RayCastMultipleCallback::RayCastMultipleCallback(uint16_t layerMask, glm::vec2 origin, float length)
        : m_LayerMask(layerMask), m_Origin(origin), m_Length(length)
    { }

    float RayCastMultipleCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
    {
        const b2Filter& filterData = fixture->GetFilterData();

        glm::vec2 pointVec = { point.x, point.y };
        if (glm::length(glm::abs(m_Origin - pointVec)) > m_Length)
            return 0.0f;

        if ((m_LayerMask & filterData.maskBits) == 0)
            return -1.0f;

        Entity entity = {};
        if (fixture != nullptr)
        {
            b2FixtureUserData userData = fixture->GetUserData();
            entity = PhysicsUtils::GetEntityFromB2DUserData(userData);
        }

        Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
        RayCastHitInfo2D hitInfo =
        {
            { point.x, point.y },
            { normal.x, normal.y },
            { physicsBody }
        };

        /*HitInfo_Internal hitInfo = 
        {
            fixture,
            { point.x, point.y },
            { normal.x, normal.y },
            fraction
        };*/

        m_HitInfos.emplace_back(hitInfo);

        return 1.0f;
    }


    /*Entity RayCastMultipleCallback::GetHitEntity(int index) const
    {
        return {};
    }

    glm::vec2 RayCastMultipleCallback::GetPoint(int index) const
    {
        return glm::vec2();
    }

    glm::vec2 RayCastMultipleCallback::GetNormal(int index) const
    {
        return glm::vec2();
    }*/

}
