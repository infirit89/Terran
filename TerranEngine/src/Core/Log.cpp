#include "trpch.h"
#include "Log.h"
#include "Time.h"

#pragma warning(push, 0)
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

#include <ctime>

namespace TerranEngine 
{
	std::unordered_map<std::string_view, Shared<spdlog::logger>>  s_Loggers;
	void Log::Init()
	{
		std::array<std::string_view, TR_CORE_LOGGER_COUNT> sinkNames
		{
			TR_LOG_CORE,
			TR_LOG_RENDERER,
			TR_LOG_SCRIPT,
			TR_LOG_ASSET,
			TR_LOG_PHYSICS
		};

		for (int32_t i = 0; i < TR_CORE_LOGGER_COUNT; i++)
		{
			std::vector<spdlog::sink_ptr> sinks
			{
				CreateShared<spdlog::sinks::stdout_color_sink_mt>(),
				CreateShared<spdlog::sinks::basic_file_sink_mt>(GetFormattedFileLoggerName("Terran"), true)
			};

			std::string pattern = "%^[%T] TERRAN [%n]: %v%$";
			sinks[0]->set_pattern(pattern);
			sinks[1]->set_pattern(pattern);

			Shared<spdlog::logger> logger = CreateShared<spdlog::logger>(std::string(sinkNames.at(i)), sinks.begin(), sinks.end());
			logger->set_level(spdlog::level::trace);
			s_Loggers.emplace(sinkNames.at(i), logger);
		}
	}

	std::string Log::GetFormattedFileLoggerName(std::string_view loggerName)
	{
		std::stringstream timeSS;
		std::tm* time = Time::GetCurrentTime();
		timeSS << "logs/" << std::put_time(time, "%m-%d-%G") << "/" 
				<< std::put_time(time, "%H_%M") << "/" << loggerName << ".log";
		return timeSS.str();
	}

	void Log::SetClientLogger(Shared<spdlog::logger> logger) 
	{
		s_Loggers.emplace(TR_LOG_CLIENT, logger);
	}

	bool Log::Contains(std::string_view loggerName)
	{
		return s_Loggers.contains(loggerName);
	}

	Shared<spdlog::logger> Log::GetLogger(std::string_view loggerName)
	{
		return s_Loggers.at(loggerName);
	}
}