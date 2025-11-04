#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace Terran {
namespace Window {

enum class MouseButton : uint8_t {
    Button1 = 0,
    Button2 = 1,
    Button3 = 2,
    Button4 = 3,
    Button5 = 4,
    Button6 = 5,
    Button7 = 6,
    Button8 = 7,

    LeftButton = Button1,
    MiddleButton = Button3,
    RightButton = Button2

};

namespace InputUtils {

constexpr inline size_t MouseButtonsSize = 8;
constexpr inline std::array<MouseButton, MouseButtonsSize> MouseButtons = {
    MouseButton::Button1,
    MouseButton::Button2,
    MouseButton::Button3,
    MouseButton::Button4,
    MouseButton::Button5,
    MouseButton::Button6,
    MouseButton::Button7,
    MouseButton::Button8
};

}
}

}
