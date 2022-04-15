#pragma once

#include "Scene/Entity.h"
#include "Scene/SceneManager.h"

#include <box2d/box2d.h>

// NOTE: to be included only in .cpp files
namespace TerranEngine 
{
	namespace PhysicsUtils 
	{
		Entity GetEntityFromB2DFixtureUserData(const b2FixtureUserData& userData);
	}
}