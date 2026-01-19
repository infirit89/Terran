#pragma once

#include "MouseButtons.h"

#include <LibCore/Event.h>

namespace Terran {
namespace Window {

class MouseMoveEvent final {
public:
    MouseMoveEvent(float const xPos, float const yPos)
        : m_XPos(xPos)
        , m_YPos(yPos)
    {
    }

    float GetX() const { return m_XPos; }
    float GetY() const { return m_YPos; }

private:
    float m_XPos;
    float m_YPos;
};

class MouseScrollEvent final {
public:
    MouseScrollEvent(float const xOffset, float const yOffset)
        : m_XOffset(xOffset)
        , m_YOffset(yOffset)
    {
    }

    float GetXOffset() const { return m_XOffset; }
    float GetYOffset() const { return m_YOffset; }

private:
    float m_XOffset;
    float m_YOffset;
};

class MouseButtonEvent {
public:
    MouseButtonEvent(MouseButton const buttonCode)
        : m_ButtonCode(buttonCode)
    {
    }

    MouseButton GetButton() const { return m_ButtonCode; }

private:
    MouseButton m_ButtonCode;
};

class MouseButtonPressedEvent final : public MouseButtonEvent {
public:
    MouseButtonPressedEvent(MouseButton const buttonCode)
        : MouseButtonEvent(buttonCode)
    {
    }
};

class MouseButtonReleasedEvent final : public MouseButtonEvent {
public:
    MouseButtonReleasedEvent(MouseButton const buttonCode)
        : MouseButtonEvent(buttonCode)
    {
    }
};

}

}
