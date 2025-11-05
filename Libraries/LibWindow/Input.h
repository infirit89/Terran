#pragma once

#include "CursorStates.h"
#include "InputState.h"
#include "KeyCodes.h"
#include "MouseButtons.h"

#include <LibCore/Log.h>
#include <LibCore/Macros.h>

#include <array>
#include <glm/glm.hpp>

#include <cstddef>
#include <cstdint>
#include <map>

namespace Terran {
namespace Window {

class Input;

class Window;
class Input final {
public:
    using key_state_map = std::map<Key, InputState>;
    using mouse_button_state_container = std::array<InputState, InputUtils::MouseButtonsSize>;

    MAKE_NONCOPYABLE(Input);
    MAKE_NONMOVEABLE(Input);

    ~Input() = default;

    constexpr bool key_pressed(Key key) const noexcept
    {
        auto const& inputState = m_keyStates.at(key);
        return !inputState.PreviousState
            && inputState.CurrentState;
    }

    constexpr bool key_down(Key key) const noexcept
    {
        return m_keyStates.at(key).CurrentState;
    }

    constexpr bool key_released(Key key) const noexcept
    {
        auto const& inputState = m_keyStates.at(key);
        return inputState.PreviousState
            && !inputState.CurrentState;
    }

    constexpr bool mouse_button_pressed(MouseButton button) const noexcept
    {
        auto const& inputState = m_mouseButtonStates.at(static_cast<size_t>(button));
        return !inputState.PreviousState
            && inputState.CurrentState;
    }

    constexpr bool mouse_button_down(MouseButton button) const noexcept
    {
        return m_mouseButtonStates.at(static_cast<size_t>(button)).CurrentState;
    }

    constexpr bool mouse_button_released(MouseButton button) const noexcept
    {
        auto const& inputState = m_mouseButtonStates.at(static_cast<size_t>(button));
        return inputState.PreviousState
            && !inputState.CurrentState;
    }

    // ------ Defined by windowing implementation ------
    bool key_state(Key key);
    bool mouse_button_state(MouseButton button);

    void set_cursor_state(CursorMode cursorMode);

    glm::vec2 mouse_position();
    // -------------------------------------------------

    void update()
    {
        for (auto& [key, keyState] : m_keyStates) {
            keyState.PreviousState = keyState.CurrentState;
            keyState.CurrentState = key_state(key);
        }

        for (uint8_t mouseButton = 0; mouseButton < InputUtils::MouseButtonsSize; mouseButton++) {
            auto& mouseButtonState = m_mouseButtonStates.at(mouseButton);
            mouseButtonState.PreviousState = mouseButtonState.CurrentState;
            mouseButtonState.CurrentState = mouse_button_state(static_cast<MouseButton>(mouseButton));
        }
    }

    constexpr key_state_map const& key_states() const& noexcept
    {
        return m_keyStates;
    }

    constexpr mouse_button_state_container const& mouse_button_states() const& noexcept
    {
        return m_mouseButtonStates;
    }

private:
    Input(Window const* window)
        : m_window(window)
    {
        for (auto const& key : InputUtils::Keys)
            m_keyStates.emplace(key, InputState());

        TR_CORE_INFO(TR_LOG_CORE, "Initialized input system");
    }

    key_state_map m_keyStates;
    mouse_button_state_container m_mouseButtonStates;
    Window const* m_window;

    friend class Window;
};

}

}
