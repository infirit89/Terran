#pragma once

#include "PhysicsBody.h"

#include "Core/Time.h"

#include "Scene/Entity.h"

#include <unordered_map>

class b2World;

namespace TerranEngine 
{
	class Physics2D
	{
	public: 
		static void CreatePhysicsWorld(const glm::vec2& gravity);
		static void CleanUpPhysicsWorld();

		static void CreatePhysicsBody(Entity entity);
		static void DestroyPhysicsBody(Entity entity);

		static void Update(Time time);

		static PhysicsBody2D& GetPhysicsBody(Entity entity);

	private:
		static b2World* s_PhysicsWorld;
		static std::unordered_map<UUID, PhysicsBody2D> s_PhysicsBodies;
	};
}