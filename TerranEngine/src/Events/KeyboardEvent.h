#pragma once

#include "Event.h"

namespace TerranEngine 
{
	class KeyboardEvent : public Event 
	{
	public:
		KeyboardEvent(const uint32_t keyCode)
			: m_KeyCode(keyCode) {}
		uint32_t GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard)
	private: 
		uint32_t m_KeyCode;
	};

	class KeyPressedEvent : public KeyboardEvent 
	{
	public:
		KeyPressedEvent(const uint32_t keyCode, const uint8_t repeatCount)
			: KeyboardEvent(keyCode), m_RepeatCount(repeatCount) {}

		uint8_t GetRepeatCount() const { return m_RepeatCount; }

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		uint8_t m_RepeatCount;
	};

	class KeyReleasedEvent : public KeyboardEvent 
	{
	public:
		KeyReleasedEvent(const uint32_t keyCode)
			: KeyboardEvent(keyCode) {}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}