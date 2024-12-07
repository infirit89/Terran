#pragma once

#include "Core/Assert.h"

#include "Scene/Entity.h"
#include "Scene/SceneManager.h"

#include <box2d/box2d.h>

// NOTE: to be included only in .cpp files
namespace TerranEngine 
{
	namespace PhysicsUtils 
	{
		template<typename T>
		UUID GetUUIDFromB2DUserData(const T& userData)
		{
            TR_ASSERT(userData.pointer, "User data is null");

			std::array<uint8_t, 16> uuidArr;
			memcpy(uuidArr._Elems, reinterpret_cast<uint8_t*>(userData.pointer), 16 * sizeof(uint8_t));
			
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
