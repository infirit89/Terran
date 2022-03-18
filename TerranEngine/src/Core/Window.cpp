#include "trpch.h"
#include "Window.h"

#include "Platform/GLFW/GLFWWindow.h"

namespace TerranEngine 
{
	Unique<Window> Window::Create(const WindowData& data)
	{
		return CreateUnique<GLFWWindow>(data);
	}
}