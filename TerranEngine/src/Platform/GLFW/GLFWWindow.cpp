#include "trpch.h"
#include "GLFWWindow.h"

#include "Core/Assert.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyboardEvent.h"
#include "Events/MouseEvent.h"
#include "Events/GamepadEvent.h"

#include "Utils/Debug/OptickProfiler.h"

#include <stb_image.h>

namespace TerranEngine 
{
	static int s_GlfwSuccess = glfwInit();
	static GLFWWindow* s_CurrentWindow = nullptr;

	GLFWWindow::GLFWWindow(WindowData data)
	{
		TR_ASSERT(s_GlfwSuccess, "GFLW couldn't initialze!");

		s_CurrentWindow = this;
		InitWindow(data);
	}
	GLFWWindow::~GLFWWindow()
	{
		Close();
	}

	void GLFWWindow::SetVsync(bool enable)
	{
		m_Vsync = enable;
		glfwSwapInterval(m_Vsync ? 1 : 0);
	}

	void GLFWWindow::SwapBuffers()
	{
		TR_PROFILE_FUNCTION();
		glfwSwapBuffers(m_Window);
	}

	void GLFWWindow::PollEvents()
	{
		TR_PROFILE_FUNCTION();
		glfwPollEvents();
	}

	void GLFWWindow::SetEventCallbackFN(const EventCallbackFn& eventCallbackFN)
	{
		m_WindowDataPtr.EventCallback = eventCallbackFN;
	}

	void GLFWWindow::SetTitle(const char* title)
	{
		glfwSetWindowTitle(m_Window, title);
	}

	void GLFWWindow::InitWindow(WindowData data)
	{
		m_WindowDataPtr.Width = data.Width;
		m_WindowDataPtr.Height = data.Height;


		if(data.Debug)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

		if(data.Maximized)
			glfwWindowHint(GLFW_MAXIMIZED, 1);
		
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		
		const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);

		m_WindowDataPtr.VideoMode = vidMode;

		m_Window = glfwCreateWindow(data.Width, data.Height, data.Name, NULL, NULL);
		TR_ASSERT(m_Window, "Couldn't create a GLFW window!");

		glfwSetWindowUserPointer(m_Window, &m_WindowDataPtr);
		
		glfwGetWindowContentScale(m_Window, &m_WindowDataPtr.XScale, &m_WindowDataPtr.YScale);
		TR_CORE_INFO(TR_LOG_CORE, "Created window");

		SetupCallbacks();
		TR_CORE_INFO(TR_LOG_CORE, "Setup window events");

		glfwMakeContextCurrent(m_Window);
		SetVsync(data.VSync);
	}

	void GLFWWindow::SetupCallbacks()
	{
		// setup event callbacks
		glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximize)
		{
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			if (maximize == 0)
			{
				int xpos = ((GLFWvidmode*)data.VideoMode)->width / 2 - data.Width / 2;
				int ypos = ((GLFWvidmode*)data.VideoMode)->height / 2 - data.Height / 2;

				glfwSetWindowPos(window, xpos, ypos);
			}
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) 
		{
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			WindowCloseEvent e;
			data.EventCallback(e);
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) 
		{
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			data.Width = width;
			data.Height = height;

			WindowResizeEvent e(width, height);
			data.EventCallback(e);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) 
		{
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent e((Key)key, 0);
				data.EventCallback(e);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent e((Key)key, 1);
				data.EventCallback(e);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent e((Key)key);
				data.EventCallback(e);
				break;
			}
			}
			});

			glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) 
			{
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			MouseMoveEvent e(static_cast<float>(xpos), static_cast<float>(ypos));

			data.EventCallback(e);
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) 
		{
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			MouseScrollEvent e(static_cast<float>(xoffset), static_cast<float>(yoffset));
			data.EventCallback(e);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) 
		{
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent e((MouseButton)button);
				data.EventCallback(e);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent e((MouseButton)button);
				data.EventCallback(e);
				break;
			}
			}
		});

		glfwSetJoystickCallback([](int joystickID, int event)
		{
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer((GLFWwindow*)s_CurrentWindow->GetNativeWindow());
			switch (event)
			{
			case GLFW_CONNECTED:
			{
				GamepadConnectedEvent e(joystickID);
				data.EventCallback(e);
				break;
			}
			case GLFW_DISCONNECTED:
			{
				GamepadDisconnectedEvent e(joystickID);
				data.EventCallback(e);
				break;
			}
			}
		});

		glfwSetWindowContentScaleCallback(m_Window, [](GLFWwindow* window, float xscale, float yscale) 
		{
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);
			WindowContentScaleChangeEvent e(xscale, yscale);
			data.EventCallback(e);
			data.XScale = xscale;
			data.YScale = yscale;
		});

	}

	void GLFWWindow::Close()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
		TR_CORE_INFO(TR_LOG_CORE, "Destroyed window and opengl context");
	}
}
