#include "RayCastCallbacks.h"
#include "trpch.h"

#include "PhysicsUtils.h"

namespace TerranEngine {

RayCastClosestCallback::RayCastClosestCallback(uint16_t layerMask)
    : m_LayerMask(layerMask)
    , m_HitInfo()
    , /*m_Fixture(nullptr), m_Point(0.0f, 0.0f), m_Normal(0.0f, 0.0f),*/ m_Fraction(0.0f)
    , m_Hit(false)
{
}

float RayCastClosestCallback::ReportFixture(b2Fixture* fixture, b2Vec2 const& point, b2Vec2 const& normal, float fraction)
{
    b2Filter const& filterData = fixture->GetFilterData();

    if ((m_LayerMask & filterData.maskBits) == 0)
        return fraction == 0.0f ? 0.0f : -1.0f;

    m_HitInfo.Normal = { normal.x, normal.y };
    m_HitInfo.Point = { point.x, point.y };

    Entity entity = {};
    if (fixture != nullptr) {
        b2FixtureUserData userData = fixture->GetUserData();
        entity = PhysicsUtils::GetEntityFromB2DUserData(userData);
    }
    m_HitInfo.PhysicsBody = Physics2D::GetPhysicsBody(entity);
    m_Fraction = fraction;

    m_Hit = true;

    return m_Fraction;
}

RayCastMultipleCallback::RayCastMultipleCallback(uint16_t layerMask)
    : m_LayerMask(layerMask)
    , m_HitInfos(Terran::Core::CreateShared<std::vector<RayCastHitInfo2D>>())
{
}

float RayCastMultipleCallback::ReportFixture(b2Fixture* fixture, b2Vec2 const& point, b2Vec2 const& normal, float fraction)
{
    b2Filter const& filterData = fixture->GetFilterData();

    if ((m_LayerMask & filterData.maskBits) == 0)
        return -1.0f;

    Entity entity = {};
    if (fixture != nullptr) {
        b2FixtureUserData userData = fixture->GetUserData();
        entity = PhysicsUtils::GetEntityFromB2DUserData(userData);
    }

    Terran::Core::Shared<PhysicsBody2D> physicsBody = Physics2D::GetPhysicsBody(entity);
    RayCastHitInfo2D hitInfo = {
        { point.x, point.y },
        { normal.x, normal.y },
        { physicsBody }
    };

    m_HitInfos->emplace_back(hitInfo);

    return 1.0f;
}

}
