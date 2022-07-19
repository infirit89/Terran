#include "trpch.h"
#include "PhysicsUtils.h"
#include "Physics.h"

namespace TerranEngine 
{
    Entity PhysicsUtils::GetEntityFromB2DFixtureUserData(const b2FixtureUserData& userData)
    {
		std::array<uint8_t, 16> uuidArr;
		memcpy(uuidArr._Elems, (uint8_t*)userData.pointer, 16 * sizeof(uint8_t));
		UUID uuid(uuidArr);

		return Physics2D::GetContext()->FindEntityWithUUID(uuid);
    }

	Entity PhysicsUtils::GetEntityFromB2DBodyUserData(const b2BodyUserData& userData)
	{
		std::array<uint8_t, 16> uuidArr;
		memcpy(uuidArr._Elems, (uint8_t*)userData.pointer, 16 * sizeof(uint8_t));
		UUID uuid(uuidArr);

		return Physics2D::GetContext()->FindEntityWithUUID(uuid);
	}
}

