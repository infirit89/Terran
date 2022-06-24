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

		return state == GLFW_REPEAT || state == GLFW_PRESS;
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

	void Input::SetCursorState(CursorState cursorState) 
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow());

		GLFWcursor* cursor = glfwCreateStandardCursor((int)cursorState);
		glfwSetCursor(window, cursor);
	}

	bool Input::IsControllerConnected(uint8_t controllerIndex)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow());

		int pressent = glfwJoystickPresent((int)controllerIndex);
		bool isGamepad = glfwJoystickIsGamepad((int)controllerIndex);

		return pressent && isGamepad;
	}

	const char* Input::GetControllerName(uint8_t controllerIndex)
	{
		if (IsControllerConnected(controllerIndex)) 
		{
			const char* controllerName = glfwGetGamepadName((int)controllerIndex);
			return controllerName;
		}

		return "";
	}

	bool Input::IsControllerButtonPressed(ControllerButton controllerButton, uint8_t controllerIndex)
	{
		GLFWgamepadstate gamepadState;

		if (glfwGetGamepadState((int)controllerIndex, &gamepadState)) 
			return gamepadState.buttons[(int)controllerButton];

		return false;
	}

	float Input::GetControllerAxis(ControllerAxis controllerAxis, uint8_t controllerIndex)
	{
		GLFWgamepadstate gamepadState;

		if (glfwGetGamepadState((int)controllerIndex, &gamepadState)) 
			return gamepadState.axes[(int)controllerAxis];

		return 0.0f;
	}

	glm::vec2 Input::GetControllerRightStick(uint8_t controllerIndex)
	{
		float rightX = GetControllerAxis(ControllerAxis::RightX, controllerIndex);
		float rightY = GetControllerAxis(ControllerAxis::RightY, controllerIndex);

		return { rightX, rightY };
	}

	glm::vec2 Input::GetControllerLeftStick(uint8_t controllerIndex)
	{
		float leftX = GetControllerAxis(ControllerAxis::LeftX, controllerIndex);
		float leftY = GetControllerAxis(ControllerAxis::LeftY, controllerIndex);

		return { leftX, leftY };
	}

	std::vector<uint8_t> Input::GetConnectedControllers()
	{
		std::vector<uint8_t> connectedControllers;

		for (uint8_t i = 0; i < GLFW_JOYSTICK_LAST; i++)
		{
			if(IsControllerConnected(i))
				connectedControllers.emplace_back(i);
		}
		
		return connectedControllers;
	}
}
