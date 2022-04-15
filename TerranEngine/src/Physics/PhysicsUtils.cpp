#include "trpch.h"
#include "PhysicsUtils.h"

namespace TerranEngine 
{
    Entity PhysicsUtils::GetEntityFromB2DFixtureUserData(const b2FixtureUserData& userData)
    {
		std::array<uint8_t, 16> uuidArr;
		memcpy(uuidArr._Elems, (uint8_t*)userData.pointer, 16 * sizeof(uint8_t));
		UUID uuid(uuidArr);

		return SceneManager::GetCurrentScene()->FindEntityWithUUID(uuid);
    }
}

