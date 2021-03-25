#include "trpch.h"

#include "GLFWWindow.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyboardEvent.h"
#include "Events/MouseEvent.h"

#include "Core/Assert.h"
#include <glad/glad.h>

namespace TerranEngine 
{
	GLFWWindow::GLFWWindow(WindowData data)
	{
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
	void GLFWWindow::Update()
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	void GLFWWindow::InitWindow(WindowData data)
	{
		m_WindowDataPtr.Width = data.Width;
		m_WindowDataPtr.Height = data.Height;

		int glfwSuccess = glfwInit();
		TR_ASSERTMSG(glfwSuccess, "GFLW couldn't initialze!");


		m_Window = glfwCreateWindow(m_WindowDataPtr.Width, m_WindowDataPtr.Height, data.Name.c_str(), NULL, NULL);

		TR_ASSERTMSG(m_Window, "Couldn't create a GLFW window!");

		glfwSetWindowUserPointer(m_Window, &m_WindowDataPtr);

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			WindowCloseEvent e;
			data.EventCallback(e);
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			data.Width = width;
			data.Height = height;

			WindowResizeEvent e(width, height);
			data.EventCallback(e);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods){
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS: 
				{
					KeyPressedEvent e(key, 0);
					data.EventCallback(e);
					break;
				}
				case GLFW_REPEAT: 
				{
					KeyPressedEvent e(key, 1);
					data.EventCallback(e);
					break;
				}
				case GLFW_RELEASE: 
				{
					KeyReleasedEvent e(key);
					data.EventCallback(e);
					break;
				}
			}
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			MouseMoveEvent e(xpos, ypos);

			data.EventCallback(e);
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			MouseScrollEvent e(xoffset, yoffset);
			data.EventCallback(e);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS: 
				{
					MouseButtonPressedEvent e(button);
					data.EventCallback(e);
					break;
				}
				case GLFW_RELEASE: 
				{
					MouseButtonReleasedEvent e(button);
					data.EventCallback(e);
					break;
				}
			}
		});

		glfwMakeContextCurrent(m_Window);
		
		int gladSuccess = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		TR_ASSERTMSG(gladSuccess, "Couldn't initialize GLAD");

		SetVsync(true);
	}

	void GLFWWindow::Close()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}
}
