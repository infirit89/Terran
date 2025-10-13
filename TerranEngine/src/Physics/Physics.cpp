#include "trpch.h"

#include "LibCore/Base.h"
#include "LibCore/UUID.h"
#include "ContatctListener.h"
#include "Physics.h"
#include "PhysicsBody.h"
#include "PhysicsLayerManager.h"
#include "PhysicsMaterial.h"
#include "RayCastCallbacks.h"

#include "Core/Settings.h"

#include "Scene/SceneManager.h"

#include "Scripting/ScriptEngine.h"

#include "Asset/AssetManager.h"

#include "Math/Math.h"

#include "Utils/Debug/OptickProfiler.h"

#include <box2d/box2d.h>

#include <glm/gtx/transform.hpp>

namespace TerranEngine {

struct PhysicsEngineState {
    b2World* PhysicsWorld = nullptr;
    std::unordered_map<Terran::Core::UUID, Terran::Core::Shared<PhysicsBody2D>> PhysicsBodies;
    float PhysicsDeltaTime = 0.0f;
    ContactListener ContactListener;
    Terran::Core::Shared<PhysicsBody2D> EmptyPhysicsBody;
    PhysicsSettings Settings;
    Terran::Core::Shared<PhysicsMaterial2D> DefaultMaterial;
};

static PhysicsEngineState* s_State;

void Physics2D::Initialize()
{
    s_State = new PhysicsEngineState();
    s_State->Settings = {};
    PhysicsLayerManager::SetLayerName(0, "Default");
    PhysicsLayerManager::SetLayerName(1, "IgnoreRayCast");
    PhysicsLayerManager::SetLayerName(3, "Test");
    s_State->DefaultMaterial = AssetManager::CreateMemoryAsset<PhysicsMaterial2D>();

    TR_CORE_INFO(TR_LOG_PHYSICS, "Initialized physics system");
}

void Physics2D::Shutdown()
{
    delete s_State;
    TR_CORE_INFO(TR_LOG_PHYSICS, "Shutdown physics system");
}

void Physics2D::CreatePhysicsWorld(PhysicsSettings const& settings)
{
    s_State->Settings = settings;
    if (s_State->PhysicsWorld) {
        TR_CORE_ERROR(TR_LOG_PHYSICS, "The existing physics world must be deleted before a new one is created");
        return;
    }

    b2Vec2 b2Gravity = { s_State->Settings.Gravity.x, s_State->Settings.Gravity.y };
    s_State->PhysicsWorld = new b2World(b2Gravity);

    s_State->PhysicsWorld->SetContactListener(&s_State->ContactListener);
    s_State->PhysicsWorld->SetAutoClearForces(true);
}

void Physics2D::CleanUpPhysicsWorld()
{
    if (!s_State->PhysicsWorld) {
        TR_CORE_ERROR(TR_LOG_PHYSICS, "Physics world is null");
        return;
    }

    s_State->PhysicsBodies.clear();

    delete s_State->PhysicsWorld;
    s_State->PhysicsWorld = nullptr;
}

void Physics2D::CratePhysicsBodies(Scene* scene)
{
    auto rigidbodyView = scene->GetEntitiesWith<Rigidbody2DComponent>();

    for (auto e : rigidbodyView) {
        Entity entity(e, scene);
        Terran::Core::Shared<PhysicsBody2D> physicsBody = Physics2D::CreatePhysicsBody(entity);
        physicsBody->AttachColliders();
    }
}

b2World* Physics2D::GetB2World() { return s_State->PhysicsWorld; }

Terran::Core::Shared<PhysicsMaterial2D> Physics2D::GetDefaultMaterial() { return s_State->DefaultMaterial; }

Terran::Core::Shared<PhysicsBody2D> Physics2D::CreatePhysicsBody(Entity entity)
{
    TR_PROFILE_FUNCTION();
    Terran::Core::Shared<PhysicsBody2D> physicsBody = Terran::Core::CreateShared<PhysicsBody2D>(entity);
    s_State->PhysicsBodies.emplace(entity.GetID(), physicsBody);
    return physicsBody;
}

void Physics2D::DestroyPhysicsBody(Entity entity)
{
    TR_PROFILE_FUNCTION();
    if (s_State->PhysicsWorld) {
        Terran::Core::UUID id = entity.GetID();
        if (s_State->PhysicsBodies.find(id) != s_State->PhysicsBodies.end()) {
            auto& physicsBody = s_State->PhysicsBodies.at(id);

            if (physicsBody)
                s_State->PhysicsBodies.erase(id);
        }
    }
}

void Physics2D::Update(Terran::Core::Time time)
{
    TR_PROFILE_FUNCTION();
    s_State->PhysicsWorld->Step(s_State->Settings.PhysicsTimestep, s_State->Settings.VelocityIterations, s_State->Settings.PositionIterations);

    s_State->PhysicsDeltaTime += time.GetDeltaTime();

    auto const scriptView = SceneManager::GetCurrentScene()->GetEntitiesWith<ScriptComponent>();

    while (s_State->PhysicsDeltaTime > 0.0f) {
        for (auto e : scriptView) {
            Entity entity(e, SceneManager::GetCurrentScene()->GetRaw());
            ScriptEngine::OnPhysicsUpdate(entity);
        }

        s_State->PhysicsDeltaTime -= s_State->Settings.PhysicsTimestep;
    }

    SyncTransforms();
}

void Physics2D::SyncTransforms()
{
    TR_PROFILE_FUNCTION();
    auto rigidbodyView = SceneManager::GetCurrentScene()->GetEntitiesWith<Rigidbody2DComponent>();

    for (auto e : rigidbodyView) {
        Entity entity(e, SceneManager::GetCurrentScene()->GetRaw());

        auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();
        auto& transform = entity.GetTransform();

        Terran::Core::Shared<PhysicsBody2D>& physicsBody = s_State->PhysicsBodies.at(entity.GetID());

        if (physicsBody->GetSleepState() == PhysicsBodySleepState::Sleep)
            continue;

        if (entity.HasParent()) {
            Entity parent = entity.GetParent();
            Terran::Core::Shared<PhysicsBody2D> parentBody = GetPhysicsBody(parent);

            b2Transform localTransform = b2MulT(parentBody->GetB2Body()->GetTransform(),
                physicsBody->GetB2Body()->GetTransform());

            transform.Position.x = localTransform.p.x;
            transform.Position.y = localTransform.p.y;
            transform.Rotation.z = localTransform.q.GetAngle();
            transform.IsDirty = true;

            continue;
        }

        transform.Position.x = physicsBody->GetPosition().x;
        transform.Position.y = physicsBody->GetPosition().y;
        transform.Rotation.z = physicsBody->GetRotation();
        transform.IsDirty = true;
    }
}

Terran::Core::Shared<PhysicsBody2D> Physics2D::GetPhysicsBody(Entity entity)
{
    if (entity && s_State->PhysicsBodies.find(entity.GetID()) != s_State->PhysicsBodies.end())
        return s_State->PhysicsBodies.at(entity.GetID());

    // return s_State->EmptyPhysicsBody;
    return nullptr;
}

bool Physics2D::RayCast(glm::vec2 const& origin, glm::vec2 const& direction, float length, RayCastHitInfo2D& hitInfo, uint16_t layerMask)
{
    TR_PROFILE_FUNCTION();
    RayCastClosestCallback raycastCallback(layerMask);
    b2Vec2 const point1 = { origin.x, origin.y };
    b2Vec2 const distance = { length * direction.x, length * direction.y };
    b2Vec2 const point2 = point1 + distance;

    s_State->PhysicsWorld->RayCast(&raycastCallback, point1, point2);

    hitInfo = raycastCallback.GetHitInfo();

    return raycastCallback.HasHit();
}

Terran::Core::Shared<std::vector<RayCastHitInfo2D>> Physics2D::RayCastAll(glm::vec2 const& origin, glm::vec2 const& direction, float length, uint16_t layerMask)
{
    TR_PROFILE_FUNCTION();
    b2Vec2 const point1 = { origin.x, origin.y };
    b2Vec2 const distance = { length * direction.x, length * direction.y };
    b2Vec2 const point2 = point1 + distance;

    RayCastMultipleCallback raycastCallback(layerMask);

    s_State->PhysicsWorld->RayCast(&raycastCallback, point1, point2);
    return raycastCallback.GetHitInfos();
}

}
