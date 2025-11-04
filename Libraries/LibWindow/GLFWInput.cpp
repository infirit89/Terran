#include "Input.h"
#include "Window.h"
#include "KeyCodes.h"
#include "MouseButtons.h"

#include <GLFW/glfw3.h>

#include <cstdint>

namespace Terran {
namespace Window {

bool Input::key_state(Key key)
{
    GLFWwindow* window = static_cast<GLFWwindow*>(m_window->native_window());

    int state = glfwGetKey(window, static_cast<int>(key));

    return state == GLFW_PRESS;
}

bool Input::mouse_button_state(MouseButton button)
{
    GLFWwindow* window = static_cast<GLFWwindow*>(m_window->native_window());

    int state = glfwGetMouseButton(window, static_cast<uint16_t>(button));

    return state == GLFW_PRESS;
}

glm::vec2 Input::mouse_position()
{
    GLFWwindow* window = static_cast<GLFWwindow*>(m_window->native_window());

    double xpos;
    double ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    return { xpos, ypos };
}

void Input::set_cursor_state(CursorMode cursorMode)
{
    GLFWwindow* window = static_cast<GLFWwindow*>(m_window->native_window());
    glfwSetInputMode(window, GLFW_CURSOR, static_cast<int>(cursorMode));
}

bool Input::controller_button_pressed(uint8_t controllerIndex, ControllerButton controllerButton)
{
    GLFWgamepadstate gamepadState;

    if (glfwGetGamepadState(controllerIndex, &gamepadState))
        return gamepadState.buttons[static_cast<int>(controllerButton)];

    return false;
}

float Input::controller_axis(uint8_t controllerIndex, ControllerAxis controllerAxis)
{
    GLFWgamepadstate gamepadState;

    if (glfwGetGamepadState(controllerIndex, &gamepadState))
        return gamepadState.axes[static_cast<int>(controllerAxis)];

    return 0.0f;
}

glm::vec2 Input::controller_right_stick(uint8_t controllerIndex)
{
    float rightX = controller_axis(controllerIndex, ControllerAxis::RightX);
    float rightY = controller_axis(controllerIndex, ControllerAxis::RightY);

    return { rightX, rightY };
}

glm::vec2 Input::controller_left_stick(uint8_t controllerIndex)
{
    float leftX = controller_axis(controllerIndex, ControllerAxis::LeftX);
    float leftY = controller_axis(controllerIndex, ControllerAxis::LeftY);

    return { leftX, leftY };
}

}
}
