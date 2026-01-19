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
// #define TR_LOG_RENDERER "Renderer"
// #define TR_LOG_SCRIPT "Script"
// #define TR_LOG_ASSET "Asset"
// #define TR_LOG_PHYSICS "Physics"
#define TR_CORE_LOGGER_COUNT 2

namespace Terran {
namespace Core {

struct LogSettings final {
    std::string name;
    spdlog::level::level_enum level;
};

class Log final {
public:
    static void init();
    static void shutdown();
    // static void SetClientLogger(Shared<spdlog::logger> logger);
    static std::string logger_file_name(std::string_view loggerName);
    static bool contains(std::string const& loggerName);
    static Shared<spdlog::logger> logger(std::string const& loggerName);
    static void set_logger_sink(std::string const& loggerName, spdlog::sink_ptr sink, size_t sinkIndex = 0);
    static void set_log_format(std::string const& loggerName, std::string const& format);
    static void add_logger(std::string const& logger_name);
};

#define TR_CORE_TRACE(LoggerName, ...)           \
    if (Terran::Core::Log::contains(LoggerName)) \
    Terran::Core::Log::logger(LoggerName)->trace(__VA_ARGS__)

#define TR_CORE_INFO(LoggerName, ...)            \
    if (Terran::Core::Log::contains(LoggerName)) \
    Terran::Core::Log::logger(LoggerName)->info(__VA_ARGS__)

#define TR_CORE_WARN(LoggerName, ...)            \
    if (Terran::Core::Log::contains(LoggerName)) \
    Terran::Core::Log::logger(LoggerName)->warn(__VA_ARGS__)

#define TR_CORE_ERROR(LoggerName, ...)           \
    if (Terran::Core::Log::contains(LoggerName)) \
    Terran::Core::Log::logger(LoggerName)->error(__VA_ARGS__)

#define TR_CLIENT_TRACE(...)                        \
    if (Terran::Core::Log::contains(TR_LOG_CLIENT)) \
    Terran::Core::Log::logger(TR_LOG_CLIENT)->trace(__VA_ARGS__)

#define TR_CLIENT_INFO(...)                         \
    if (Terran::Core::Log::contains(TR_LOG_CLIENT)) \
    Terran::Core::Log::logger(TR_LOG_CLIENT)->info(__VA_ARGS__)

#define TR_CLIENT_WARN(...)                         \
    if (Terran::Core::Log::contains(TR_LOG_CLIENT)) \
    Terran::Core::Log::logger(TR_LOG_CLIENT)->warn(__VA_ARGS__)

#define TR_CLIENT_ERROR(...)                        \
    if (Terran::Core::Log::contains(TR_LOG_CLIENT)) \
    Terran::Core::Log::logger(TR_LOG_CLIENT)->error(__VA_ARGS__)

}

}
