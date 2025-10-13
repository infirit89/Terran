#pragma once

#include "LibCore/Assert.h"
#include "LibCore/UUID.h"

#include "Scene/Entity.h"
#include "Scene/SceneManager.h"

#include <array>
#include <box2d/box2d.h>
#include <cstdint>
#include <cstring>

// NOTE: to be included only in .cpp files
namespace TerranEngine {
namespace PhysicsUtils {

template<typename T>
Terran::Core::UUID GetUUIDFromB2DUserData(T const& userData)
{
    TR_ASSERT(userData.pointer, "User data is null");

    std::array<uint8_t, 16> uuidArr;
    memcpy(uuidArr.data(), reinterpret_cast<uint8_t*>(userData.pointer), 16 * sizeof(uint8_t));

    return { uuidArr };
}

template<typename T>
Entity GetEntityFromB2DUserData(T const& userData)
{
    Terran::Core::UUID id = GetUUIDFromB2DUserData(userData);
    return SceneManager::GetCurrentScene()->FindEntityWithUUID(id);
}

}
}
