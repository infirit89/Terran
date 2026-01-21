/**
 * @file Time.h
 * @brief Time utilities
 * @ingroup LibCore
 */
#pragma once

#include <ctime>

namespace Terran::Core {

/**
 * @class Time
 * @brief Mainly used to get delta time during an update cycle
 */
class Time final {
public:
    Time(float time)
        : m_time(time)
    {
    }

    float delta_time_milliseconds() const { return m_time * 1000.0f; }
    float delta_time() const { return m_time; }

    static std::tm* current_time()
    {
        std::time_t time = std::time(nullptr);
        return std::localtime(&time);
    }

private:
    float m_time = 0.0f;
};

}
