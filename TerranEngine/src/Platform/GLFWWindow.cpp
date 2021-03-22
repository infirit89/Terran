#include "GLFWWindow.h"
#include "Core/Log.h"

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
		m_Data.Width = data.Width;
		m_Data.Height = data.Height;

		if (!glfwInit()) TR_ERROR("glfw couldn't initialize");

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, data.Name.c_str(), NULL, NULL);
		if (!m_Window) TR_ERROR("couldn't create a glfw window");

		glfwMakeContextCurrent(m_Window);

		SetVsync(true);
	}
	void GLFWWindow::Close()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}
}
