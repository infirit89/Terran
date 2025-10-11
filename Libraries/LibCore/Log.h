#pragma once

#include "Base.h"
#include "UUID.h"

#include <glm/detail/qualifier.hpp>
#include <glm/detail/setup.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#pragma warning(push, 0)
#include <spdlog/common.h>
#include <spdlog/fmt/bundled/core.h>
#include <spdlog/fmt/bundled/format.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/logger.h>
#pragma warning(pop)

#include <array>
#include <cstddef>
#include <cstdint>
#include <ios>
#include <string>
#include <string_view>

template<glm::length_t L, typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<L, T, Q>> {
    static constexpr auto parse(format_parse_context const& ctx) -> decltype(ctx.begin())
    {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(glm::vec<1, T, Q> const& vec, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{}", vec[0]);
    }

    template<typename FormatContext>
    auto format(glm::vec<2, T, Q> const& vec, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{}, {}", vec[0], vec[1]);
    }

    template<typename FormatContext>
    auto format(glm::vec<3, T, Q> const& vec, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{}, {}, {}", vec[0], vec[1], vec[2]);
    }

    template<typename FormatContext>
    auto format(glm::vec<4, T, Q> const& vec, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{}, {}, {}, {}", vec[0], vec[1], vec[2], vec[3]);
    }
};

template<typename T, glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>> {
    static constexpr auto parse(format_parse_context const& ctx) -> decltype(ctx.begin())
    {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(glm::qua<T, Q> const& qua, FormatContext& ctx) -> decltype(ctx.out())
    {
        return format_to(ctx.out(), "{}, {}, {}, {}", qua.x, qua.y, qua.z, qua.w);
    }
};

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

template<typename OStream>
OStream& operator<<(OStream& os, const UUID& uuid)
{
    std::array<uint8_t, 16> idArr = uuid.GetData();

    int ind = 0;
    for (int i = 0; i < 20; i++) {
        if (i == 4 || i == 7 || i == 10 || i == 13) {
            os << '-';
            continue;
        }

        os << std::hex
           << (idArr[ind] >> 4 & 0x0f)
           << (int)(idArr[ind] & 0x0f);

        ind++;
    }

    return os;
}

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
