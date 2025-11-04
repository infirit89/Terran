#include "WindowSystem.h"

#include <LibCore/Layer.h>
#include <LibCore/Result.h>

#include <GLFW/glfw3.h>
#include <LibCore/Time.h>
#include <LibWindow/ControllerInput.h>

namespace Terran {
namespace Window {

WindowSystem::WindowSystem()
    : Core::Layer("WindowLayer")
{
}

WindowSystem::~WindowSystem()
{
}

Core::Result<void> WindowSystem::on_attach()
{
    // we want hats and buttons to be queried using different functions
    // without this querying a joystick buttons' state will also give us
    // its hats' state
    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
    if (glfwInit() != GLFW_TRUE)
        return Core::Bad();

    m_controllerInput.initialize();

    // maybe not the greates solution???
    // for now though i still want the users to be able to access the controller input through the WindowSystem
    static ControllerInput* ControllerInputInst = &m_controllerInput;
    glfwSetJoystickCallback([](int id, int event) {
        if (event == GLFW_CONNECTED)
            ControllerInputInst->init_controller(id);
        else
            ControllerInputInst->shutdown_controller(id);
    });

    return {};
}
Core::Result<void> WindowSystem::on_dettach()
{
    // if glfwInit was unsuccessful running this function does nothing
    // thus there is no need to conditionally run it
    glfwTerminate();
    return {};
}

void WindowSystem::update(Core::Time&)
{
    for (auto& [id, window] : m_windows) {
        window->poll_events();
    }

    m_controllerInput.update();
}

}
}
