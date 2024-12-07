#pragma once

#include <box2d/b2_world_callbacks.h>

namespace TerranEngine 
{
	class ContactListener final : public b2ContactListener 
	{
	public:
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;

		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
		void PostSolve(b2Contact* contact, const b2ContactImpulse* contactImpulse) override;
	};
}