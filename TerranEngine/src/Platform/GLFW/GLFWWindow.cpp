#include "trpch.h"
#include "GLFWWindow.h"

#include "Core/Assert.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyboardEvent.h"
#include "Events/MouseEvent.h"

#include <stb_image.h>

#pragma warning (push)
#pragma warning (disable : 4244)

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

	void GLFWWindow::SetTitle(const char* title)
	{
		glfwSetWindowTitle(m_Window, title);
	}

	void GLFWWindow::InitWindow(WindowData data)
	{
		m_WindowDataPtr.Width = data.Width;
		m_WindowDataPtr.Height = data.Height;

		int glfwSuccess = glfwInit();
		TR_ASSERT(glfwSuccess, "GFLW couldn't initialze!");
		
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

		if(data.Maximized)
			glfwWindowHint(GLFW_MAXIMIZED, 1);
		
		GLFWmonitor* montitor = glfwGetPrimaryMonitor();
		
		const GLFWvidmode* vidMode = glfwGetVideoMode(montitor);

		m_WindowDataPtr.VideoMode = vidMode;
		m_Window = glfwCreateWindow(m_WindowDataPtr.Width, m_WindowDataPtr.Height, data.Name, NULL, NULL);

		TR_ASSERT(m_Window, "Couldn't create a GLFW window!");

		// NOTE: bad bad bad very bad
#if 0
		GLFWimage iconImages[2];
		iconImages[0].pixels = stbi_load(data.IconPaths[0].c_str(), &iconImages[0].width, &iconImages[0].height, 0, 4);
		iconImages[1].pixels = stbi_load(data.IconPaths[1].c_str(), &iconImages[1].width, &iconImages[1].height, 0, 4);

		glfwSetWindowIcon(m_Window, 2, iconImages);
		stbi_image_free(iconImages[0].pixels);
		stbi_image_free(iconImages[1].pixels);
#endif

		glfwSetWindowUserPointer(m_Window, &m_WindowDataPtr);

		// setup event callbacks
		glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int miximize) 
		{
			WindowDataPtr& data = *(WindowDataPtr*)glfwGetWindowUserPointer(window);

			if (miximize == 0)
			{
				float xpos = ((GLFWvidmode*)data.VideoMode)->width / 2 - data.Width / 2;
				float ypos = ((GLFWvidmode*)data.VideoMode)->height / 2 - data.Height / 2;

				glfwSetWindowPos(window, xpos, ypos);
			}
		});

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
		
		glfwMakeContextCurrent(m_Window);
		SetVsync(data.VSync);
	}

	void GLFWWindow::Close()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}
}
#pragma warning (pop)
