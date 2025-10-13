#pragma once

#include "LibCore/Base.h"
#include "LibCore/Time.h"

#include "PhysicsBody.h"

#include "Scene/Scene.h"

#include "PhysicsMaterial.h"
#include <cstdint>

class b2World;

namespace TerranEngine {

struct PhysicsSettings final {
    glm::vec2 Gravity = { 0.0f, -9.81f };
    int32_t VelocityIterations = 6;
    int32_t PositionIterations = 2;
    float PhysicsTimestep = 0.02f;
};

struct RayCastHitInfo2D final {
    glm::vec2 Point = { 0.0f, 0.0f };
    glm::vec2 Normal = { 0.0f, 0.0f };
    Terran::Core::Shared<PhysicsBody2D> PhysicsBody;

    bool operator<(RayCastHitInfo2D const& other) const { return glm::all(glm::lessThan(Point, other.Point)); }
};

class Physics2D final {
public:
    static void Initialize();
    static void Shutdown();

    static void CreatePhysicsWorld(PhysicsSettings const& settings);
    static void CleanUpPhysicsWorld();

    static void CratePhysicsBodies(Scene* scene);

    static Terran::Core::Shared<PhysicsBody2D> CreatePhysicsBody(Entity entity);
    static void DestroyPhysicsBody(Entity entity);

    static void Update(Terran::Core::Time time);
    static void SyncTransforms();

    static b2World* GetB2World();

    static Terran::Core::Shared<PhysicsMaterial2D> GetDefaultMaterial();

    static Terran::Core::Shared<PhysicsBody2D> GetPhysicsBody(Entity entity);
    static bool RayCast(glm::vec2 const& origin, glm::vec2 const& direction, float length, RayCastHitInfo2D& hitInfo, uint16_t layerMask);
    static Terran::Core::Shared<std::vector<RayCastHitInfo2D>> RayCastAll(glm::vec2 const& origin, glm::vec2 const& direction, float length, uint16_t layerMask);
};

}
