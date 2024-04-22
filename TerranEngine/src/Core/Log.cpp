#include "trpch.h"
#include "Log.h"
#include "Time.h"

#pragma warning(push, 0)
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

#include <yaml-cpp/yaml.h>

#include <ctime>
#include <filesystem>

namespace TerranEngine 
{
	static std::unordered_map<std::string_view, Shared<spdlog::logger>>  s_Loggers;
	static std::array<LogSettings, TR_CORE_LOGGER_COUNT> s_Settings
	{
		LogSettings { Name = TR_LOG_CORE },
		LogSettings { Name = TR_LOG_RENDERER },
		LogSettings { Name = TR_LOG_SCRIPT },
		LogSettings { Name = TR_LOG_ASSET },
		LogSettings { Name = TR_LOG_PHYSICS },
	};

	static std::filesystem::path s_SettingsPath = "Resources/LogSettings.settings";

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

	void Log::Shutdown()
	{
	}

	static spdlog::level::level_enum GetLogLevel(const std::string& logLevel) 
	{
		if (logLevel == "trace") return spdlog::level::level_enum::trace;
		if (logLevel == "info") return spdlog::level::level_enum::info;
		if (logLevel == "warn") return spdlog::level::level_enum::warn;
		if (logLevel == "err") return spdlog::level::level_enum::err;

		TR_CORE_WARN(TR_LOG_CORE, "Invalid log level, defaulting to trace");
		return spdlog::level::level_enum::trace;
	}

	static std::string_view LogLevelToString(spdlog::level::level_enum logLevel) 
	{
		switch (logLevel)
		{
		case spdlog::level::trace: return "trace";
		case spdlog::level::info: return "info";
		case spdlog::level::warn: return "warn";
		case spdlog::level::err: return "err";
		case spdlog::level::off: return "off";
		}

		TR_CORE_ERROR(TR_LOG_CORE, "Invalid log level");
		return nullptr;
	}

	static void LoadLogSettings() 
	{
		YAML::Node node;

		try 
		{
			node = YAML::LoadFile(s_SettingsPath.string());
		}
		catch (const YAML::Exception& e) 
		{
			TR_CORE_ERROR(TR_LOG_CORE, e.what());
			return;
		}

		try 
		{
			auto logSettings = node["Settings"];

			if (!logSettings)
				return;

			for (size_t i = 0; i < logSettings.size(); i++)
			{
				LogSettings& settings = s_Settings.at(i);
				settings.Level = GetLogLevel(logSettings[i].as<std::string>(""));
				
			}
		}
		catch (const YAML::BadSubscript& e) 
		{
			TR_CORE_ERROR(TR_LOG_CORE, e.what());
			return;
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
