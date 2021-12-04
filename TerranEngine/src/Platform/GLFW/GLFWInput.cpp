#include "trpch.h"

#include "Core/Input.h"
#include "Core/Application.h"

#include <GLFW/glfw3.h>

namespace TerranEngine 
{
	bool Input::IsKeyPressed(Key key)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow());

		int state = glfwGetKey(window, (uint32_t)key);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(MouseButton button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow());

		int state = glfwGetMouseButton(window, (uint16_t)button);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	glm::vec2 Input::GetMousePos() 
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow());

		double xpos, ypos;

		glfwGetCursorPos(window, &xpos, &ypos);

		return { xpos, ypos };
	}
}