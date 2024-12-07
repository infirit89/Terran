#pragma once

#include "Event.h"

#include "Core/MouseButtons.h"

namespace TerranEngine 
{

	class MouseMoveEvent final : public Event
	{
	public:
		MouseMoveEvent(const float xPos, const float yPos) 
			: m_XPos(xPos), m_YPos(yPos) {}

		float GetX() const { return m_XPos; }
		float GetY() const { return m_YPos; }

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse)
	private:
		float m_XPos; float m_YPos;
	};

	class MouseScrollEvent final : public Event
	{
	public:
		MouseScrollEvent(const float xOffset, const float yOffset) 
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse)

	private:
		float m_XOffset; float m_YOffset;
	};

	class MouseButtonEvent : public Event
	{
	public:
		MouseButtonEvent(const MouseButton buttonCode)
			: m_ButtonCode(buttonCode) {}

		MouseButton GetButton() const { return m_ButtonCode; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse)
	private:
		MouseButton m_ButtonCode;
	};

	class MouseButtonPressedEvent final : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const MouseButton buttonCode)
			: MouseButtonEvent(buttonCode) {}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent final : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseButton buttonCode)
			: MouseButtonEvent(buttonCode) {}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}