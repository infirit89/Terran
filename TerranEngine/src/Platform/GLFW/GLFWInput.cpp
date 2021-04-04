#include "trpch.h"

#include "Core/Input.h"
#include "Core/Application.h"

#include <GLFW/glfw3.h>

namespace TerranEngine 
{
	bool Input::IsKeyPressed(uint32_t key)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow());

		int state = glfwGetKey(window, key);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(uint16_t button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow());

		int state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
}