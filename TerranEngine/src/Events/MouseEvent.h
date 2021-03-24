#pragma once
#include "Event.h"

namespace TerranEngine 
{

	class MouseMoveEvent : public Event 
	{
	public:
		MouseMoveEvent(const float xpos, const float ypos) 
			: m_XPos(xpos), m_YPos(ypos) {}

		float GetX() const { return m_XPos; }
		float GetY() const { return m_YPos; }

		EVENT_CLASS_TYPE(MouseMoved)
	private:
		float m_XPos; float m_YPos;
	};

	class MouseScrollEvent : public Event 
	{
	public:
		MouseScrollEvent(const float xoffset, const float yoffset) 
			: m_XOffset(xoffset), m_YOffset(yoffset) {}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		EVENT_CLASS_TYPE(MouseScrolled)

	private:
		float m_XOffset; float m_YOffset;
	};

	class MouseButtonEvent : public Event 
	{
	public:
		MouseButtonEvent(const int buttonCode) 
			: m_ButtonCode(buttonCode) {}

		int GetButton() const { return m_ButtonCode; }
	private:
		int m_ButtonCode;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent 
	{
	public:
		MouseButtonPressedEvent(const int buttonCode) 
			: MouseButtonEvent(buttonCode) {}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const int buttonCode)
			: MouseButtonEvent(buttonCode) {}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}