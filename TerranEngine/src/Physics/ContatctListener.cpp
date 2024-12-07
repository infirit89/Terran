#include "trpch.h"
#include "ContatctListener.h"
#include "PhysicsUtils.h"

#include "Scene/Entity.h"

#include "Scripting/ScriptEngine.h"

namespace TerranEngine 
{
	void ContactListener::BeginContact(b2Contact* contact)
	{
		const b2Fixture* collidee = contact->GetFixtureA();

		const b2Fixture* collider = contact->GetFixtureB();

		b2FixtureUserData userDataA = collidee->GetUserData();
		b2FixtureUserData userDataB = collider->GetUserData();

		Entity entityA = PhysicsUtils::GetEntityFromB2DUserData(userDataA);
		Entity entityB = PhysicsUtils::GetEntityFromB2DUserData(userDataB);

		if (entityA && entityB) 
		{
			ScriptEngine::OnPhysicsBeginContact(entityA, entityB);
			ScriptEngine::OnPhysicsBeginContact(entityB, entityA);
		}
	}

	void ContactListener::EndContact(b2Contact* contact)
	{
		const b2Fixture* collidee = contact->GetFixtureA();

		const b2Fixture* collider = contact->GetFixtureB();

		b2FixtureUserData userDataA = collidee->GetUserData();
		b2FixtureUserData userDataB = collider->GetUserData();

		Entity entityA = PhysicsUtils::GetEntityFromB2DUserData(userDataA);
		Entity entityB = PhysicsUtils::GetEntityFromB2DUserData(userDataB);

		if (entityA && entityB) 
		{
			ScriptEngine::OnPhysicsEndContact(entityA, entityB);
			ScriptEngine::OnPhysicsEndContact(entityB, entityA);
		}
	}

	void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
	{
	}

	void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* contactImpulse)
	{
	}
}
