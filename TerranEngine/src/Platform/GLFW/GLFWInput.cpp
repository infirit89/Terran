#include "trpch.h"

#include "Core/Input.h"
#include "Core/Application.h"

#include <GLFW/glfw3.h>

namespace TerranEngine 
{
	bool Input::GetKeyState(Key key)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow());

		int state = glfwGetKey(window, static_cast<uint32_t>(key));

		return state == GLFW_PRESS;
	}

	bool Input::GetMouseButtonState(MouseButton button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow());

		int state = glfwGetMouseButton(window, static_cast<uint16_t>(button));

		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePos() 
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow());

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { xpos, ypos };
	}

	void Input::SetCursorState(CursorMode cursorMode) 
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow());
		glfwSetInputMode(window, GLFW_CURSOR, static_cast<int>(cursorMode));
	}

	bool Input::IsControllerConnected(uint8_t controllerIndex)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow());

		const int present = glfwJoystickPresent(controllerIndex);
		bool isGamePad = glfwJoystickIsGamepad(controllerIndex);

		return present && isGamePad;
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

		if (glfwGetGamepadState(controllerIndex, &gamepadState)) 
			return gamepadState.buttons[static_cast<int>(controllerButton)];

		return false;
	}

	float Input::GetControllerAxis(ControllerAxis controllerAxis, uint8_t controllerIndex)
	{
		GLFWgamepadstate gamepadState;

		if (glfwGetGamepadState(controllerIndex, &gamepadState)) 
			return gamepadState.axes[static_cast<int>(controllerAxis)];

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
		connectedControllers.reserve(GLFW_JOYSTICK_LAST);

		for (uint8_t i = 0; i < GLFW_JOYSTICK_LAST; i++)
		{
			if(IsControllerConnected(i))
				connectedControllers.emplace_back(i);
		}
		
		return connectedControllers;
	}
}
