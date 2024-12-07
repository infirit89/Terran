#pragma once

#include "Core/Time.h"
#include "Core/Base.h"

#include "PhysicsBody.h"

#include "Scene/Scene.h"

#include "PhysicsMaterial.h"

class b2World;

namespace TerranEngine 
{
    struct PhysicsSettings final
    {
        glm::vec2 Gravity = { 0.0f, -9.81f };
        int32_t VelocityIterations = 6;
        int32_t PositionIterations = 2;
        float PhysicsTimestep = 0.02f;
    };

	struct RayCastHitInfo2D final
	{
		glm::vec2 Point = { 0.0f, 0.0f };
		glm::vec2 Normal = { 0.0f, 0.0f };
		Shared<PhysicsBody2D> PhysicsBody;

		bool operator <(const RayCastHitInfo2D& other) const { return glm::all(glm::lessThan(Point, other.Point)); }
	};

	class Physics2D final
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void CreatePhysicsWorld(const PhysicsSettings& settings);
		static void CleanUpPhysicsWorld();

        static void CratePhysicsBodies(Scene* scene);

		static Shared<PhysicsBody2D> CreatePhysicsBody(Entity entity);
		static void DestroyPhysicsBody(Entity entity);

		static void Update(Time time);
		static void SyncTransforms();

		static b2World* GetB2World();

		static Shared<PhysicsMaterial2D> GetDefaultMaterial();

		static Shared<PhysicsBody2D> GetPhysicsBody(Entity entity);
		static bool RayCast(const glm::vec2& origin, const glm::vec2& direction, float length, RayCastHitInfo2D& hitInfo, uint16_t layerMask);
		static Shared<std::vector<RayCastHitInfo2D>> RayCastAll(const glm::vec2& origin, const glm::vec2& direction, float length, uint16_t layerMask);
	};
}

