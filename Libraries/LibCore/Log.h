#pragma once

#include "Base.h"

#pragma warning(push, 0)
#include <spdlog/common.h>
#include <spdlog/fmt/bundled/core.h>
#include <spdlog/fmt/bundled/format.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/logger.h>
#pragma warning(pop)

#include <cstddef>
#include <string>
#include <string_view>

#define TR_LOG_CLIENT "Client"
#define TR_LOG_CORE "Core"
#define TR_LOG_RENDERER "Renderer"
#define TR_LOG_SCRIPT "Script"
#define TR_LOG_ASSET "Asset"
#define TR_LOG_PHYSICS "Physics"
#define TR_CORE_LOGGER_COUNT 6

namespace Terran {
namespace Core {

struct LogSettings final {
    std::string Name;
    spdlog::level::level_enum Level;
};

class Log final {
public:
    static void Init();
    static void Shutdown();
    // static void SetClientLogger(Shared<spdlog::logger> logger);
    static std::string GetFormattedFileLoggerName(std::string_view loggerName);
    static bool Contains(std::string const& loggerName);
    static Shared<spdlog::logger> GetLogger(std::string const& loggerName);
    static void SetLoggerSink(std::string const& loggerName, spdlog::sink_ptr sink, size_t sinkIndex = 0);
    static void SetLogFormat(std::string const& loggerName, std::string const& format);
};

#define TR_CORE_TRACE(LoggerName, ...)           \
    if (Terran::Core::Log::Contains(LoggerName)) \
    Terran::Core::Log::GetLogger(LoggerName)->trace(__VA_ARGS__)

#define TR_CORE_INFO(LoggerName, ...)            \
    if (Terran::Core::Log::Contains(LoggerName)) \
    Terran::Core::Log::GetLogger(LoggerName)->info(__VA_ARGS__)

#define TR_CORE_WARN(LoggerName, ...)            \
    if (Terran::Core::Log::Contains(LoggerName)) \
    Terran::Core::Log::GetLogger(LoggerName)->warn(__VA_ARGS__)

#define TR_CORE_ERROR(LoggerName, ...)           \
    if (Terran::Core::Log::Contains(LoggerName)) \
    Terran::Core::Log::GetLogger(LoggerName)->error(__VA_ARGS__)

#define TR_CLIENT_TRACE(...)                        \
    if (Terran::Core::Log::Contains(TR_LOG_CLIENT)) \
    Terran::Core::Log::GetLogger(TR_LOG_CLIENT)->trace(__VA_ARGS__)

#define TR_CLIENT_INFO(...)                         \
    if (Terran::Core::Log::Contains(TR_LOG_CLIENT)) \
    Terran::Core::Log::GetLogger(TR_LOG_CLIENT)->info(__VA_ARGS__)

#define TR_CLIENT_WARN(...)                         \
    if (Terran::Core::Log::Contains(TR_LOG_CLIENT)) \
    Terran::Core::Log::GetLogger(TR_LOG_CLIENT)->warn(__VA_ARGS__)

#define TR_CLIENT_ERROR(...)                        \
    if (Terran::Core::Log::Contains(TR_LOG_CLIENT)) \
    Terran::Core::Log::GetLogger(TR_LOG_CLIENT)->error(__VA_ARGS__)

}

}
