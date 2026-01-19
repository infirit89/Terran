#pragma once
#include <LibCore/Event.h>

#include <cstdint>

namespace Terran {
namespace Window {

class GamePadEvent {
public:
    GamePadEvent(uint8_t const gamePadId)
        : m_GamePadId(gamePadId)
    {
    }

    uint8_t GetGamePadId() const { return m_GamePadId; }

private:
    uint8_t m_GamePadId;
};

class GamePadConnectedEvent final : public GamePadEvent {
public:
    GamePadConnectedEvent(uint8_t const gamePadId)
        : GamePadEvent(gamePadId)
    {
    }
};

class GamePadDisconnectedEvent final : public GamePadEvent {
public:
    GamePadDisconnectedEvent(uint8_t const gamePadId)
        : GamePadEvent(gamePadId)
    {
    }
};

}

}
