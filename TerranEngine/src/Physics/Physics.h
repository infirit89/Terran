#pragma once

#include "PhysicsBody.h"

#include "Core/Time.h"

#include "Scene/Scene.h"

namespace TerranEngine 
{
	struct RayCastHitInfo2D 
	{
		glm::vec2 Point;
		glm::vec2 Normal;
		PhysicsBody2D PhysicsBody;
	};

	class Physics2D
	{
	public:
		static void Initialize();
		static void Shutdown();
		
		static void CreatePhysicsWorld(const glm::vec2& gravity);
		static void CleanUpPhysicsWorld();

		static void CreatePhysicsBody(Entity entity);
		static void DestroyPhysicsBody(Entity entity);

		static void Update(Time time);

		static void SetContext(const Shared<Scene>& context);
		static Shared<Scene>& GetContext();
		
		static PhysicsBody2D& GetPhysicsBody(Entity entity);
		static bool RayCast(const glm::vec2& origin, const glm::vec2& direction, float length, RayCastHitInfo2D& hitInfo);
	};
}
