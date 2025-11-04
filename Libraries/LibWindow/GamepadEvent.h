#pragma once
#include <LibCore/Event.h>

#include <cstdint>

namespace Terran {
namespace Window {

class GamePadEvent : public Terran::Core::Event {
public:
    GamePadEvent(uint8_t const gamePadId)
        : m_GamePadId(gamePadId)
    {
    }

    uint8_t GetGamePadId() const { return m_GamePadId; }

    EVENT_CLASS_CATEGORY(EventCategoryGamePad)
private:
    uint8_t m_GamePadId;
};

class GamePadConnectedEvent final : public GamePadEvent {
public:
    GamePadConnectedEvent(uint8_t const gamePadId)
        : GamePadEvent(gamePadId)
    {
    }
    EVENT_CLASS_TYPE(GamePadConnected)
};

class GamePadDisconnectedEvent final : public GamePadEvent {
public:
    GamePadDisconnectedEvent(uint8_t const gamePadId)
        : GamePadEvent(gamePadId)
    {
    }

    EVENT_CLASS_TYPE(GamePadDisconnected)
};

}

}
