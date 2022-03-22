#pragma once


#include "EditorPanels/LogPanel.h"
#include <spdlog/sinks/base_sink.h>
#include <mutex>

namespace TerranEditor 
{
	class EditorConsoleSink : public spdlog::sinks::base_sink<std::mutex>
	{
	public:
		EditorConsoleSink() 
		{
			m_MessageBuffer.reserve(1);
		}

		~EditorConsoleSink() = default;

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override 
		{
			spdlog::memory_buf_t formatted;
			spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);

			LogMessage logMessage;
			logMessage.Message = fmt::to_string(formatted);
			logMessage.MessageLevel = GetMessageLevel(msg.level);

			m_MessageBuffer.push_back(logMessage);

			flush_();
		}

		void flush_() override 
		{
			for (const auto& logMessage : m_MessageBuffer)
			{
				LogPanel::GetInstance()->AddLog(logMessage);
			}

			m_MessageBuffer.clear();
		}

	private:
		LogMessageLevel GetMessageLevel(spdlog::level::level_enum level) 
		{
			switch (level)
			{
			case spdlog::level::trace:
			case spdlog::level::debug:
			case spdlog::level::info:
				return LogMessageLevel::Info;
				break;
			case spdlog::level::warn:
				return LogMessageLevel::Warn;
				break;
			case spdlog::level::err:
			case spdlog::level::critical:
				return LogMessageLevel::Error;
				break;
			}
		}

		std::vector<LogMessage> m_MessageBuffer;
	};
}