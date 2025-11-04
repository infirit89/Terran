#include "ControllerInput.h"

#include <GLFW/glfw3.h>

#include <LibCore/Assert.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace Terran {
namespace Window {

void ControllerInput::initialize()
{
    for (uint8_t controllerIndex = 0; controllerIndex < InputUtils::MaxSupportedControllers; controllerIndex++) {
        init_controller(controllerIndex);
    }
}
void ControllerInput::update()
{

    for (uint8_t controllerIndex = 0; controllerIndex < InputUtils::MaxSupportedControllers; controllerIndex++) {
        Controller& controller = m_controllers.at(controllerIndex);

        if (controller.state() != ControllerState::Connected)
            continue;

        int buttonCount = 0;
        uint8_t const* buttons = glfwGetJoystickButtons(controllerIndex, &buttonCount);

        for (size_t buttonIndex = 0; std::min((size_t)buttonCount, controller.m_buttonStates.size()); buttonIndex++) {
            auto& buttonState = controller.m_buttonStates[buttonIndex];

            buttonState.PreviousState = buttonState.PreviousState;
            buttonState.CurrentState = buttons[buttonIndex];
        }
    }
}

void ControllerInput::init_controller(uint8_t id)
{
    TR_ASSERT(id < InputUtils::MaxSupportedControllers, "Controller Id exceeds max supported controller count - 15");
    Controller& controller = m_controllers.at(id);
    controller.m_id = id;
    bool is_connected = glfwJoystickPresent(id);
    controller.m_state = static_cast<ControllerState>(is_connected);
    if (!is_connected)
        return;

    controller.m_name = glfwGetJoystickName(id);
    int axesCount = 0;
    float const* axesRaw = glfwGetJoystickAxes(id, &axesCount);
    int buttonCount = 0;
    glfwGetJoystickButtons(id, &buttonCount);

    if (axesCount > (uint8_t)ControllerAxis::Last
        || buttonCount > (uint8_t)ControllerButton::Last) {
        controller.m_state = ControllerState::Unsupported;
        return;
    }

    // axis is a std::span thus a view and so it will just store the pointer
    // that is returned by glfwGetJoystickAxes in a type safe way
    // so we don't need to also query it every update
    // also this needs to be set to "empty_axis" when the controller disconnects
    // as GLFW Doc: "The returned array is allocated and freed by GLFW.
    // You should not free it yourself. It is valid until the specified joystick is disconnected or the library is terminated."
    controller.m_axis = Controller::axis_container(axesRaw, axesCount);
}

void ControllerInput::shutdown_controller(uint8_t id)
{
    TR_ASSERT(id < InputUtils::MaxSupportedControllers, "Controller Id exceeds max supported controller count - 15");
    Controller& controller = m_controllers.at(id);
    if (controller.state() == ControllerState::Disconnected)
        return;

    controller.m_state = ControllerState::Disconnected;
    controller.m_axis = Controller::empty_axis;
    controller.m_buttonStates = {};
}

}
}
