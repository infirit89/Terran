#pragma once

#include "Scene/Entity.h"
#include "Scene/SceneManager.h"

#include <box2d/box2d.h>

// NOTE: to be included only in .cpp files
namespace TerranEngine 
{
	namespace PhysicsUtils 
	{
		// NOTE: consider switching to c++20 because this code can benefit from concepts

		template<typename T>
		UUID GetUUIDFromB2DUserData(const T& userData)
		{
			std::array<uint8_t, 16> uuidArr;
			memcpy(uuidArr._Elems, (uint8_t*)userData.pointer, 16 * sizeof(uint8_t));
			
			return { uuidArr };
		}

		template<typename T>
		Entity GetEntityFromB2DUserData(const T& userData) 
		{
			UUID id = GetUUIDFromB2DUserData(userData);
			return SceneManager::GetCurrentScene()->FindEntityWithUUID(id);
		}


	}
}