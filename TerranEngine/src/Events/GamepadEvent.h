#pragma once
#include "Event.h"

namespace TerranEngine 
{
	class GamePadEvent : public Event
	{
	public:
		GamePadEvent(const uint8_t gamePadId) 
			: m_GamePadId(gamePadId)
		{}

		uint8_t GetGamePadId() const { return m_GamePadId; }

		EVENT_CLASS_CATEGORY(EventCategoryGamePad)
	private:
		uint8_t m_GamePadId;
	};

	class GamePadConnectedEvent final : public GamePadEvent
	{
	public:
		GamePadConnectedEvent(const uint8_t gamePadId) 
			: GamePadEvent(gamePadId)
		{}
		EVENT_CLASS_TYPE(GamePadConnected)
	};

	class GamePadDisconnectedEvent final : public GamePadEvent
	{
	public:
		GamePadDisconnectedEvent(const uint8_t gamePadId)
			: GamePadEvent(gamePadId)
		{}

		EVENT_CLASS_TYPE(GamePadDisconnected)
	};
}
