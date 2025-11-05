#pragma once

#include "InputState.h"

#include <LibCore/Assert.h>
#include <LibCore/Log.h>
#include <LibCore/Macros.h>

#include <glm/vec2.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>

namespace Terran {
namespace Window {

enum class ControllerButton : uint8_t {
    ButtonA = 0,
    ButtonB,
    ButtonX,
    ButtonY,
    LeftBumper,
    RightBumper,
    Back,
    Start,
    Guide,
    LeftThumb,
    RightThumb,
    DPadUp,
    DPadRight,
    DPadDown,
    DPadLeft,
    Last,
    ButtonCross = ButtonA,
    ButtonCircle = ButtonB,
    ButtonSquare = ButtonX,
    ButtonTriangle = ButtonY
};

enum class ControllerAxis : uint8_t {
    LeftX = 0,
    LeftY,
    RightX,
    RightY,
    LeftTrigger,
    RightTrigger,
    Last
};

enum class ControllerState : uint8_t {
    Disconnected = 0,
    Connected,
    Unsupported
};

namespace InputUtils {

// NOTE: this comes from glfw's max supported joystick count
constexpr inline uint8_t MaxSupportedControllers = 15;

}

class ControllerInput;

struct Controller final {
public:
    using button_container = std::array<InputState, static_cast<size_t>(ControllerButton::Last)>;

    using axis_container = std::span<float const, static_cast<size_t>(ControllerAxis::Last)>;

    constexpr static std::array<float const, static_cast<size_t>(ControllerAxis::Last)> empty_axis {};

    Controller()
        : m_axis(empty_axis)
    {
    }
    constexpr uint32_t id() const noexcept
    {
        return m_id;
    }

    constexpr std::string_view name() const noexcept
    {
        return m_name;
    }

    constexpr ControllerState state() const noexcept
    {
        return m_state;
    }

    constexpr float axis(ControllerAxis controllerAxis) const noexcept
    {
        return m_axis[static_cast<size_t>(controllerAxis)];
    }

    constexpr glm::vec2 right_stick() const noexcept
    {
        float rightX = axis(ControllerAxis::RightX);
        float rightY = axis(ControllerAxis::RightY);

        return { rightX, rightY };
    }

    constexpr glm::vec2 left_axis() const noexcept
    {
        float leftX = axis(ControllerAxis::LeftX);
        float leftY = axis(ControllerAxis::LeftY);

        return { leftX, leftY };
    }

    constexpr bool is_button_down(ControllerButton button) const noexcept
    {
        return m_buttonStates[static_cast<size_t>(button)].CurrentState;
    }

    constexpr bool is_button_released(ControllerButton button) const noexcept
    {
        InputState const& inputState = m_buttonStates[static_cast<size_t>(button)];
        return inputState.PreviousState
            && !inputState.CurrentState;
    }

    constexpr bool is_button_pressed(ControllerButton button) const noexcept
    {
        InputState const& inputState = m_buttonStates[static_cast<size_t>(button)];
        return !inputState.PreviousState
            && inputState.CurrentState;
    }

    // a little utility enum, based on the fact that only 15 controllers are supported
    enum : uint8_t {
        One = 0,
        Two,
        Three,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Nine,
        Ten,
        Eleven,
        Twelve,
        Thirteen,
        Fourteen,
        Fifteen,
    };

private:
    uint32_t m_id;
    std::string m_name;
    button_container m_buttonStates;
    axis_container m_axis;
    ControllerState m_state;
    friend class ControllerInput;
};

class ControllerInput final {
public:
    using controller_state_container = std::array<Controller, InputUtils::MaxSupportedControllers>;

    ControllerInput() = default;

    void initialize();

    void update();

    constexpr controller_state_container const& data() noexcept
    {
        return m_controllers;
    }

    constexpr Controller const& at(uint8_t id) noexcept
    {
        TR_ASSERT(id < InputUtils::MaxSupportedControllers, "Controller Id exceeds max supported controller count - 15");
        return m_controllers.at(id);
    }

    controller_state_container::iterator begin() noexcept {
        return m_controllers.begin();
    }

    controller_state_container::const_iterator begin() const noexcept {
        return m_controllers.begin();
    }

    controller_state_container::const_iterator cbegin() const noexcept {
        return m_controllers.cbegin();
    }

    controller_state_container::iterator end() noexcept {
        return m_controllers.end();
    }

    controller_state_container::const_iterator end() const noexcept {
        return m_controllers.end();
    }

    controller_state_container::const_iterator cend() const noexcept {
        return m_controllers.cend();
    }

    void init_controller(uint8_t id);
    void shutdown_controller(uint8_t id);

private:
    controller_state_container m_controllers;
};

}

}
