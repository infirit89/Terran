#include "trpch.h"
#include "ContatctListener.h"

#include "Core/Log.h"
#include "Core/UUID.h"

#include "Scene/Scene.h"
#include "Scene/SceneManager.h"
#include "Scene/Entity.h"

#include "Scripting/ScriptEngine.h"

namespace TerranEngine 
{
	static Entity GetEntityFromB2DFixtureUserData(const b2FixtureUserData& userData) 
	{
		std::array<uint8_t, 16> uuidArr;
		memcpy(uuidArr._Elems, (uint8_t*)userData.pointer, 16 * sizeof(uint8_t));
		UUID uuid(uuidArr);

		return SceneManager::GetCurrentScene()->FindEntityWithUUID(uuid);
	}

	void ContactListener::BeginContact(b2Contact* contact)
	{
		const b2Fixture* collidee = contact->GetFixtureA();

		const b2Fixture* collider = contact->GetFixtureB();

		b2FixtureUserData userDataA = collidee->GetUserData();
		b2FixtureUserData userDataB = collider->GetUserData();

		Entity entityA = GetEntityFromB2DFixtureUserData(userDataA);
		Entity entityB = GetEntityFromB2DFixtureUserData(userDataB);

		if (entityA && entityB) 
			ScriptEngine::PhysicsBeginContact(entityA, entityB);
	}

	void ContactListener::EndContact(b2Contact* contact)
	{
		const b2Fixture* collidee = contact->GetFixtureA();

		const b2Fixture* collider = contact->GetFixtureB();

		b2FixtureUserData userDataA = collidee->GetUserData();
		b2FixtureUserData userDataB = collider->GetUserData();

		Entity entityA = GetEntityFromB2DFixtureUserData(userDataA);
		Entity entityB = GetEntityFromB2DFixtureUserData(userDataB);

		if (entityA && entityB)
			ScriptEngine::PhysicsEndContact(entityA, entityB);

	}

	void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{
	}

	void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* contactImpulse)
	{
	}
}
