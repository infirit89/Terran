#pragma once

#include "Event.h"

#include <spdlog/common.h>
#include <string_view>
namespace TerranEngine 
{
	class ScriptEngineLogEvent : public Event
	{
	public:
		ScriptEngineLogEvent(std::string_view message, spdlog::level::level_enum level)
			: m_Message(message), m_Level(level) {}

		EVENT_CLASS_TYPE(CustomEvent)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		std::string_view GetLogMessage() const { return m_Message; }
		spdlog::level::level_enum GetLevel() const { return m_Level; }

	private:
		std::string_view m_Message;
		spdlog::level::level_enum m_Level;
	};
}
