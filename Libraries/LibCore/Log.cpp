#include "Log.h"
#include "Time.h"

#pragma warning(push, 0)
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#pragma warning(pop)

#include <yaml-cpp/yaml.h>

#include <ctime>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

namespace Terran {
namespace Core {

static std::unordered_map<std::string, Shared<spdlog::logger>> s_Loggers;
static std::array<std::string, TR_CORE_LOGGER_COUNT> s_LoggersNames {
    TR_LOG_CORE,
    TR_LOG_RENDERER,
    TR_LOG_SCRIPT,
    TR_LOG_ASSET,
    TR_LOG_PHYSICS,
    TR_LOG_CLIENT
};

static std::filesystem::path s_SettingsPath = "Resources/LogSettings.settings";

static spdlog::level::level_enum GetLogLevel(std::string const& logLevel)
{
    if (logLevel == "trace")
        return spdlog::level::level_enum::trace;
    if (logLevel == "info")
        return spdlog::level::level_enum::info;
    if (logLevel == "warn")
        return spdlog::level::level_enum::warn;
    if (logLevel == "err")
        return spdlog::level::level_enum::err;
    if (logLevel == "off")
        return spdlog::level::level_enum::off;

    TR_CORE_WARN(TR_LOG_CORE, "Invalid log level, defaulting to trace");
    return spdlog::level::level_enum::trace;
}

static std::string LogLevelToString(spdlog::level::level_enum logLevel)
{
    switch (logLevel) {
    case spdlog::level::trace:
    case spdlog::level::debug:
        return "trace";
    case spdlog::level::info:
        return "info";
    case spdlog::level::warn:
        return "warn";
    case spdlog::level::err:
    case spdlog::level::critical:
        return "err";
    case spdlog::level::off:
        return "off";
    default:
        return "trace";
    }

    TR_CORE_WARN(TR_LOG_CORE, "Invalid log level, defaulting to trace");
    return "trace";
}

static void LoadLogSettings()
{
    YAML::Node node;

    try {
        node = YAML::LoadFile(s_SettingsPath.string());
    } catch (YAML::Exception const& e) {
        TR_CORE_ERROR(TR_LOG_CORE, e.what());
        return;
    }

    try {
        auto loggersSettings = node["Loggers"];

        if (!loggersSettings)
            return;

        size_t index = 0;
        for (auto const& logName : s_LoggersNames) {
            auto logSettings = loggersSettings[index];
            s_Loggers.at(logName)->set_level(GetLogLevel(logSettings[logName]["Level"].as<std::string>("")));
            index++;
        }
    } catch (YAML::BadSubscript const& e) {
        TR_CORE_ERROR(TR_LOG_CORE, e.what());
        return;
    }
}

static void WriteLogSettings()
{
    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Key << "Loggers" << YAML::BeginSeq;

    for (auto const& name : s_LoggersNames) {
        out << YAML::BeginMap;
        out << YAML::Key << name << YAML::BeginMap;

        out << YAML::Key << "Level" << YAML::Value << LogLevelToString(s_Loggers.at(name)->level());

        out << YAML::EndMap;
        out << YAML::EndMap;
    }

    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream ofs(s_SettingsPath);
    ofs << out.c_str();
}

void Log::Init()
{
    for (auto const& name : s_LoggersNames) {
        std::vector<spdlog::sink_ptr> sinks {
            CreateShared<spdlog::sinks::stdout_color_sink_mt>(),
            CreateShared<spdlog::sinks::basic_file_sink_mt>(GetFormattedFileLoggerName(name), true)
        };

        std::string pattern = "%^[%T] TERRAN [%n]: %v%$";
        sinks[0]->set_pattern(pattern);
        sinks[1]->set_pattern(pattern);

        Shared<spdlog::logger> logger = CreateShared<spdlog::logger>(name, sinks.begin(), sinks.end());
        logger->set_level(spdlog::level::trace);
        s_Loggers.emplace(name, logger);
    }

    LoadLogSettings();
}

void Log::Shutdown()
{
    WriteLogSettings();
}

std::string Log::GetFormattedFileLoggerName(std::string_view loggerName)
{
    std::stringstream timeSS;
    std::tm* time = Time::GetCurrentTime();
    timeSS << "logs/" << std::put_time(time, "%m-%d-%G") << "/"
           << std::put_time(time, "%H_%M") << "/" << loggerName << ".log";
    return timeSS.str();
}

/*void Log::SetClientLogger(Shared<spdlog::logger> logger)
{
        s_Loggers.emplace(TR_LOG_CLIENT, logger);
}*/

bool Log::Contains(std::string const& loggerName)
{
    return s_Loggers.contains(loggerName);
}

Shared<spdlog::logger> Log::GetLogger(std::string const& loggerName)
{
    return s_Loggers.at(loggerName);
}

void Log::SetLoggerSink(std::string const& loggerName, spdlog::sink_ptr sink, size_t sinkIndex)
{
    s_Loggers.at(loggerName)->sinks().at(sinkIndex) = sink;
}

void Log::SetLogFormat(std::string const& loggerName, std::string const& format)
{
    s_Loggers.at(loggerName)->set_pattern(format);
}
}


}
