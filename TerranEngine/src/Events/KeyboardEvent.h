#pragma once

#include "Core/KeyCodes.h"
#include "Event.h"

namespace TerranEngine 
{
	class KeyboardEvent : public Event 
	{
	public:
		KeyboardEvent(const Key keyCode)
			: m_KeyCode(keyCode) {}
		Key GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard)
	private: 
		Key m_KeyCode;
	};

	class KeyPressedEvent : public KeyboardEvent 
	{
	public:
		KeyPressedEvent(const Key keyCode, const uint8_t repeatCount)
			: KeyboardEvent(keyCode), m_RepeatCount(repeatCount) 
		{
		}

		uint8_t GetRepeatCount() const { return m_RepeatCount; }
		EVENT_CLASS_TYPE(KeyPressed)
	private:
		uint8_t m_RepeatCount;
	};

	class KeyReleasedEvent : public KeyboardEvent 
	{
	public:
		KeyReleasedEvent(const Key keyCode)
			: KeyboardEvent(keyCode) {}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}