#include "Window.h"
#include "trpch.h"

#include "Platform/GLFW/GLFWWindow.h"

namespace TerranEngine {

Terran::Core::Unique<Window> Window::Create(WindowData const& data)
{
    return Terran::Core::CreateUnique<GLFWWindow>(data);
}

}

