#pragma once
#include "Event.h"

#include "Core/MouseButtons.h"

namespace TerranEngine 
{

	class MouseMoveEvent : public Event 
	{
	public:
		MouseMoveEvent(const float xpos, const float ypos) 
			: m_XPos(xpos), m_YPos(ypos) {}

		inline float GetX() const { return m_XPos; }
		inline float GetY() const { return m_YPos; }

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse)
	private:
		float m_XPos; float m_YPos;
	};

	class MouseScrollEvent : public Event 
	{
	public:
		MouseScrollEvent(const float xoffset, const float yoffset) 
			: m_XOffset(xoffset), m_YOffset(yoffset) {}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

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

		inline MouseButton GetButton() const { return m_ButtonCode; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse)
	private:
		MouseButton m_ButtonCode;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent 
	{
	public:
		MouseButtonPressedEvent(const MouseButton buttonCode)
			: MouseButtonEvent(buttonCode) {}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseButton buttonCode)
			: MouseButtonEvent(buttonCode) {}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}