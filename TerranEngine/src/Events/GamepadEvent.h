#pragma once
#include "Event.h"

namespace TerranEngine 
{
	class GamepadEvent : public Event
	{
	public:
		GamepadEvent(const uint8_t gamepadId) 
			: m_GamepadId(gamepadId)
		{}

		uint8_t GetGamepadID() const { return m_GamepadId; }

		EVENT_CLASS_CATEGORY(EventCategoryGamepad)
	private:
		uint8_t m_GamepadId;
	};

	class GamepadConnectedEvent : public GamepadEvent
	{
	public:
		GamepadConnectedEvent(const uint8_t gamepadId) 
			: GamepadEvent(gamepadId)
		{}
		EVENT_CLASS_TYPE(GamepadConnected)
	};

	class GamepadDisconnectedEvent : public GamepadEvent 
	{
	public:
		GamepadDisconnectedEvent(const uint8_t gamepadId) 
			: GamepadEvent(gamepadId)
		{}

		EVENT_CLASS_TYPE(GamepadDisconnected)
	};
}
