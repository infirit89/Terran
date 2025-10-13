#pragma once

#include "LibCore/Event.h"

#include "Core/MouseButtons.h"

namespace TerranEngine {

class MouseMoveEvent final : public Terran::Core::Event {
public:
    MouseMoveEvent(float const xPos, float const yPos)
        : m_XPos(xPos)
        , m_YPos(yPos)
    {
    }

    float GetX() const { return m_XPos; }
    float GetY() const { return m_YPos; }

    EVENT_CLASS_TYPE(MouseMoved)
    EVENT_CLASS_CATEGORY(EventCategoryMouse)
private:
    float m_XPos;
    float m_YPos;
};

class MouseScrollEvent final : public Terran::Core::Event {
public:
    MouseScrollEvent(float const xOffset, float const yOffset)
        : m_XOffset(xOffset)
        , m_YOffset(yOffset)
    {
    }

    float GetXOffset() const { return m_XOffset; }
    float GetYOffset() const { return m_YOffset; }

    EVENT_CLASS_TYPE(MouseScrolled)
    EVENT_CLASS_CATEGORY(EventCategoryMouse)

private:
    float m_XOffset;
    float m_YOffset;
};

class MouseButtonEvent : public Terran::Core::Event {
public:
    MouseButtonEvent(MouseButton const buttonCode)
        : m_ButtonCode(buttonCode)
    {
    }

    MouseButton GetButton() const { return m_ButtonCode; }

    EVENT_CLASS_CATEGORY(EventCategoryMouse)
private:
    MouseButton m_ButtonCode;
};

class MouseButtonPressedEvent final : public MouseButtonEvent {
public:
    MouseButtonPressedEvent(MouseButton const buttonCode)
        : MouseButtonEvent(buttonCode)
    {
    }

    EVENT_CLASS_TYPE(MouseButtonPressed)
};

class MouseButtonReleasedEvent final : public MouseButtonEvent {
public:
    MouseButtonReleasedEvent(MouseButton const buttonCode)
        : MouseButtonEvent(buttonCode)
    {
    }

    EVENT_CLASS_TYPE(MouseButtonReleased)
};

}

