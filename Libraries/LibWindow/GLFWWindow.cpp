#include "GLFWWindow.h"

#include "LibCore/Assert.h"

#include "GamepadEvent.h"
#include "KeyboardEvent.h"
#include "MouseEvent.h"
#include "WindowEvent.h"

namespace Terran {
namespace Window {
namespace Implementation {

static int s_GlfwSuccess = glfwInit();
static GLFWwindow* s_CurrentWindow = nullptr;

GLFWWindow::GLFWWindow(WindowData const& data)
{
    TR_ASSERT(s_GlfwSuccess, "GFLW couldn't initialze!");

    init(data);
}
GLFWWindow::~GLFWWindow()
{
    destroy();
}

void GLFWWindow::set_vsync(bool enable)
{
    m_Vsync = enable;
    glfwSwapInterval(m_Vsync ? 1 : 0);
}

void GLFWWindow::swap_buffers()
{
    glfwSwapBuffers(m_Window);
}

void GLFWWindow::poll_events()
{
    glfwPollEvents();
}

void GLFWWindow::set_event_callback(EventCallbackFn const& eventCallbackFN)
{
    m_WindowDataPtr.EventCallback = eventCallbackFN;
}

void GLFWWindow::set_title(std::string_view title)
{
    glfwSetWindowTitle(m_Window, title.data());
}

void GLFWWindow::init(WindowData data)
{
    m_WindowDataPtr.Width = data.Width;
    m_WindowDataPtr.Height = data.Height;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (data.Debug)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    if (data.Maximized)
        glfwWindowHint(GLFW_MAXIMIZED, 1);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    GLFWvidmode const* vidMode = glfwGetVideoMode(monitor);

    m_WindowDataPtr.VideoMode = vidMode;

    m_Window = glfwCreateWindow(data.Width, data.Height, data.Name.data(), nullptr, nullptr);
    s_CurrentWindow = m_Window;
    TR_ASSERT(m_Window, "Couldn't create a GLFW window!");

    glfwSetWindowUserPointer(m_Window, &m_WindowDataPtr);

    glfwGetWindowContentScale(m_Window, &m_WindowDataPtr.XScale, &m_WindowDataPtr.YScale);
    TR_CORE_INFO(TR_LOG_CORE, "Created window");

    setup_callbacks();
    TR_CORE_INFO(TR_LOG_CORE, "Setup window events");

    glfwMakeContextCurrent(m_Window);
    set_vsync(data.VSync);
}

void GLFWWindow::setup_callbacks()
{
    // setup event callbacks
    glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximize) {
        WindowDataPtr& data = *static_cast<WindowDataPtr*>(glfwGetWindowUserPointer(window));

        if (maximize == 0) {
            int xpos = ((GLFWvidmode*)data.VideoMode)->width / 2 - data.Width / 2;
            int ypos = ((GLFWvidmode*)data.VideoMode)->height / 2 - data.Height / 2;

            glfwSetWindowPos(window, xpos, ypos);
        }
    });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
        WindowDataPtr& data = *static_cast<WindowDataPtr*>(glfwGetWindowUserPointer(window));

        WindowCloseEvent e;
        data.EventCallback(e);
    });

    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
        WindowDataPtr& data = *static_cast<WindowDataPtr*>(glfwGetWindowUserPointer(window));

        data.Width = width;
        data.Height = height;

        WindowResizeEvent e(width, height);
        data.EventCallback(e);
    });

    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        WindowDataPtr& data = *static_cast<WindowDataPtr*>(glfwGetWindowUserPointer(window));

        switch (action) {
        case GLFW_PRESS: {
            KeyPressedEvent e(static_cast<Key>(key), 0);
            data.EventCallback(e);
            break;
        }
        case GLFW_REPEAT: {
            KeyPressedEvent e(static_cast<Key>(key), 1);
            data.EventCallback(e);
            break;
        }
        case GLFW_RELEASE: {
            KeyReleasedEvent e(static_cast<Key>(key));
            data.EventCallback(e);
            break;
        }
        }
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
        WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

        MouseMoveEvent e(static_cast<float>(xpos), static_cast<float>(ypos));

        data.EventCallback(e);
    });

    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
        WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

        MouseScrollEvent e(static_cast<float>(xoffset), static_cast<float>(yoffset));
        data.EventCallback(e);
    });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
        WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

        switch (action) {
        case GLFW_PRESS: {
            MouseButtonPressedEvent e((MouseButton)button);
            data.EventCallback(e);
            break;
        }
        case GLFW_RELEASE: {
            MouseButtonReleasedEvent e((MouseButton)button);
            data.EventCallback(e);
            break;
        }
        }
    });

    // FIXME: this should not belong to a window
    glfwSetJoystickCallback([](int joystickID, int event) {
        WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(s_CurrentWindow);
        switch (event) {
        case GLFW_CONNECTED: {
            GamePadConnectedEvent e(joystickID);
            data.EventCallback(e);
            break;
        }
        case GLFW_DISCONNECTED: {
            GamePadDisconnectedEvent e(joystickID);
            data.EventCallback(e);
            break;
        }
        }
    });

    glfwSetWindowContentScaleCallback(m_Window, [](GLFWwindow* window, float xscale, float yscale) {
        WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);
        WindowContentScaleChangeEvent e(xscale, yscale);
        data.EventCallback(e);
        data.XScale = xscale;
        data.YScale = yscale;
    });

    glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused) {
        if (focused)
            s_CurrentWindow = window;
    });
}

void GLFWWindow::destroy()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
    TR_CORE_INFO(TR_LOG_CORE, "Destroyed window and opengl context");
}

}

}

}
