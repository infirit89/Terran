#pragma once

#include "KeyCodes.h"

#include <LibCore/Event.h>

#include <cstdint>

namespace Terran {
namespace Window {

class KeyboardEvent {
public:
    KeyboardEvent(Key const keyCode)
        : m_KeyCode(keyCode)
    {
    }

    Key GetKeyCode() const { return m_KeyCode; }

private:
    Key m_KeyCode;
};

class KeyPressedEvent final : public KeyboardEvent {
public:
    KeyPressedEvent(Key const keyCode, uint8_t const repeatCount)
        : KeyboardEvent(keyCode)
        , m_RepeatCount(repeatCount)
    {
    }

    uint8_t GetRepeatCount() const { return m_RepeatCount; }
private:
    uint8_t m_RepeatCount;
};

class KeyReleasedEvent final : public KeyboardEvent {
public:
    KeyReleasedEvent(Key const keyCode)
        : KeyboardEvent(keyCode)
    {
    }

};

}

}
