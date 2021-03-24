#include "trpch.h"

#include "Window.h"
#include "Platform/GLFWWindow.h"

namespace TerranEngine 
{
	std::unique_ptr<Window> Window::Create(const WindowData& data)
	{
		return std::make_unique<GLFWWindow>(std::forward<const WindowData&>(data));
	}
}