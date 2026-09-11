#pragma once

#include <string_view>
#include <spdlog/common.h>

namespace Terran::Script {

class ScriptEngineLogEvent final {
public:
    ScriptEngineLogEvent(std::string_view message, spdlog::level::level_enum level)
        : m_message(message)
        , m_level(level)
    {
    }

    std::string_view log_message() const { return m_message; }
    spdlog::level::level_enum level() const { return m_level; }

private:
    std::string_view m_message;
    spdlog::level::level_enum m_level;
};

}
