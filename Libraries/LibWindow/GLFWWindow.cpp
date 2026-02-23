#include "GLFWWindow.h"

#include "KeyboardEvent.h"
#include "MouseEvent.h"
#include "WindowEvent.h"
#include "Window.h"
#include "WindowTypes.h"
#include "MouseButtons.h"
#include "KeyCodes.h"

#include <LibCore/Assert.h>
#include <LibCore/Event.h>
#include <LibCore/Log.h>

#include <GLFW/glfw3.h>

#include <string_view>

namespace Terran {
namespace Window {
namespace Implementation {

static GLFWwindow* s_current_window = nullptr;

GLFWWindow::GLFWWindow(Core::EventDispatcher& event_dispatcher, WindowData const& data)
    : Window(event_dispatcher)
{
    init(data);
}
GLFWWindow::~GLFWWindow()
{
    destroy();
}

void GLFWWindow::set_vsync(bool enable)
{
    m_vsync = enable;
    glfwSwapInterval(m_vsync ? 1 : 0);
}

void GLFWWindow::swap_buffers()
{
    glfwSwapBuffers(m_window);
}

void GLFWWindow::poll_events()
{
    glfwPollEvents();
}

void GLFWWindow::set_title(std::string_view title)
{
    glfwSetWindowTitle(m_window, title.data());
}

void GLFWWindow::init(WindowData data)
{
    m_window_data_ptr.width = data.Width;
    m_window_data_ptr.height = data.Height;
    m_window_data_ptr.event_dispatcher = &event_dispatcher;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (data.Debug)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    if (data.Maximized)
        glfwWindowHint(GLFW_MAXIMIZED, 1);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    GLFWvidmode const* vidMode = glfwGetVideoMode(monitor);

    m_window_data_ptr.video_mode = vidMode;

    m_window = glfwCreateWindow(data.Width, data.Height, data.Name.data(), nullptr, nullptr);
    TR_ASSERT(m_window, "Couldn't create a GLFW window!");
    s_current_window = m_window;

    glfwSetWindowUserPointer(m_window, &m_window_data_ptr);

    glfwGetWindowContentScale(m_window, &m_window_data_ptr.scale_x, &m_window_data_ptr.scale_y);
    TR_INFO(TR_LOG_WINDOW, "Created window {} {}x{}", data.Name, data.Width, data.Height);

    TR_TRACE(TR_LOG_WINDOW, "Setting up window event handlers");
    setup_callbacks();
    TR_INFO(TR_LOG_WINDOW, "Window event handlers successfuly setup");

    glfwMakeContextCurrent(m_window);
    set_vsync(data.VSync);
}

void GLFWWindow::setup_callbacks()
{
    // setup event callbacks
    glfwSetWindowMaximizeCallback(m_window, [](GLFWwindow* window, int maximize) {
        WindowDataPtr& data = *static_cast<WindowDataPtr*>(glfwGetWindowUserPointer(window));

        if (maximize == 0) {
            int xpos = ((GLFWvidmode*)data.video_mode)->width / 2 - data.width / 2;
            int ypos = ((GLFWvidmode*)data.video_mode)->height / 2 - data.height / 2;

            glfwSetWindowPos(window, xpos, ypos);
        }
    });

    glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
        WindowDataPtr& data = *static_cast<WindowDataPtr*>(glfwGetWindowUserPointer(window));

        WindowCloseEvent e;
        data.event_dispatcher->trigger(e);
    });

    glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        WindowDataPtr& data = *static_cast<WindowDataPtr*>(glfwGetWindowUserPointer(window));

        data.width = width;
        data.height = height;

        WindowResizeEvent e(width, height);
        data.event_dispatcher->trigger(e);
        // invokeEventCallback(data.EventCallback, e);
    });

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        WindowDataPtr& data = *static_cast<WindowDataPtr*>(glfwGetWindowUserPointer(window));

        switch (action) {
        case GLFW_PRESS: {
            KeyPressedEvent e(static_cast<Key>(key), 0);
            data.event_dispatcher->trigger(e);
            break;
        }
        case GLFW_REPEAT: {
            KeyPressedEvent e(static_cast<Key>(key), 1);
            data.event_dispatcher->trigger(e);
            break;
        }
        case GLFW_RELEASE: {
            KeyReleasedEvent e(static_cast<Key>(key));
            data.event_dispatcher->trigger(e);
            break;
        }
        }
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
        WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

        MouseMoveEvent e(static_cast<float>(xpos), static_cast<float>(ypos));

        data.event_dispatcher->trigger(e);
    });

    glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
        WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

        MouseScrollEvent e(static_cast<float>(xoffset), static_cast<float>(yoffset));
        data.event_dispatcher->trigger(e);
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
        WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

        switch (action) {
        case GLFW_PRESS: {
            MouseButtonPressedEvent e((MouseButton)button);
            data.event_dispatcher->trigger(e);
            break;
        }
        case GLFW_RELEASE: {
            MouseButtonReleasedEvent e((MouseButton)button);
            data.event_dispatcher->trigger(e);
            break;
        }
        }
    });


    glfwSetWindowContentScaleCallback(m_window, [](GLFWwindow* window, float xscale, float yscale) {
        WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);
        WindowContentScaleChangeEvent e(xscale, yscale);
        data.event_dispatcher->trigger(e);
        data.scale_x = xscale;
        data.scale_y = yscale;
    });

    glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused) {
        if (focused)
            s_current_window = window;
    });
}

void GLFWWindow::destroy()
{
    glfwDestroyWindow(m_window);
    TR_INFO(TR_LOG_WINDOW, "Destroyed window");
}

}

}

}
