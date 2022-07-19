#pragma once

#include "Scene/Entity.h"

#include <box2d/box2d.h>

// NOTE: to be included only in .cpp files
namespace TerranEngine 
{
	namespace PhysicsUtils 
	{
		// NOTE: make templated?
		Entity GetEntityFromB2DFixtureUserData(const b2FixtureUserData& userData);
		Entity GetEntityFromB2DBodyUserData(const b2BodyUserData& userData);
	}
}