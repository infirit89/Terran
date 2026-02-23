#include "WindowSystem.h"
#include "ControllerInput.h"
#include "GamepadEvent.h"
#include "WindowTypes.h"

#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/Layer.h>
#include <LibCore/Log.h>
#include <LibCore/Result.h>
#include <LibCore/Time.h>

#include <GLFW/glfw3.h>

namespace Terran {
namespace Window {

// NOTE: this is needed for the glfwSetJoystickCallback
// i dont like it either but it is what it is
// maybe consider using glfwpp???
static Core::RawPtr<Core::EventDispatcher> s_event_dispatcher;
WindowSystem::WindowSystem(Core::EventDispatcher& dispatcher)
    : Core::Layer("WindowLayer", dispatcher)
{
    s_event_dispatcher = &dispatcher;
    Core::Log::add_logger(TR_LOG_WINDOW);
}

WindowSystem::~WindowSystem()
{
}

static void window_error_callback(int error, char const* description)
{
    TR_ERROR(TR_LOG_WINDOW, "{}: {}", error, description);
}
Core::Result<void> WindowSystem::on_attach()
{
    // we want hats and buttons to be queried using different functions
    // without this querying a joystick buttons' state will also give us
    // its hats' state
    TR_TRACE(TR_LOG_WINDOW, "Intializing GLFW...");
    glfwSetErrorCallback(window_error_callback);
    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
    if (glfwInit() != GLFW_TRUE) {
        char const* error_description;
        int error_code = glfwGetError(&error_description);
        if (error_description) {
            TR_ERROR(TR_LOG_WINDOW, "Couldn't initialize GLFW! {}: {}", error_code, error_description);
        } else {
            TR_ERROR(TR_LOG_WINDOW, "Couldn't initialize GLFW! {}", error_code);
        }
        return Core::Bad();
    }

    m_controllerInput.initialize();
    TR_INFO(TR_LOG_WINDOW, "GLFW successfuly initialized!");

    // maybe not the greatest solution???
    // for now though i still want the users to be able to access the controller input through the WindowSystem
    static ControllerInput* ControllerInputInst = &m_controllerInput;
    glfwSetJoystickCallback([](int id, int event) {
        if (event == GLFW_CONNECTED) {

            GamePadConnectedEvent e(id);
            s_event_dispatcher->trigger(e);
            ControllerInputInst->init_controller(id);
        } else {

            GamePadDisconnectedEvent e(id);
            s_event_dispatcher->trigger(e);
            ControllerInputInst->shutdown_controller(id);
        }
    });

    return {};
}
Core::Result<void> WindowSystem::on_dettach()
{
    // if glfwInit was unsuccessful running this function does nothing
    // thus there is no need to conditionally run it
    m_windows.clear();

    TR_TRACE(TR_LOG_WINDOW, "Terminating GLFW...");
    glfwTerminate();
    TR_INFO(TR_LOG_WINDOW, "GLFW successfuly terminated!");
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
