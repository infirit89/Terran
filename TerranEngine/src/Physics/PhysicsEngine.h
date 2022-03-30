#pragma once

#include "Core/Time.h"

#include "Scene/Entity.h"

class b2World;

namespace TerranEngine 
{
	class PhysicsEngine 
	{
	public: 
		static void CreatePhysicsWorld(const glm::vec2& gravity);
		static void CleanUpPhysicsWorld();

		static void CreateRigidbody(Entity entity);
		static void DestroyRigidbody(Entity entity);

		static void CreateBoxCollider(Entity entity);
		static void CreateCircleCollider(Entity entity);

		static void Update(Time time);

	private:
		static b2World* s_PhysicsWorld;
	};
}