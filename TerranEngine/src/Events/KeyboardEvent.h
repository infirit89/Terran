#pragma once

#include "Event.h"

namespace TerranEngine 
{
	class KeyboardEvent : public Event 
	{
	public:
		KeyboardEvent(const int keyCode) 
			: m_KeyCode(keyCode) {}
		int GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard)
	private: 
		int m_KeyCode;
	};

	class KeyPressedEvent : public KeyboardEvent 
	{
	public:
		KeyPressedEvent(const int keyCode, const int repeatCount)
			: KeyboardEvent(keyCode), m_RepeatCount(repeatCount) {}

		int GetRepeatCount() const { return m_RepeatCount; }

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};

	class KeyReleasedEvent : public KeyboardEvent 
	{
	public:
		KeyReleasedEvent(const int keyCode)
			: KeyboardEvent(keyCode) {}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}