#pragma once

#include "KeyCodes.h"
#include "MouseButtons.h"
#include "CursorStates.h"
#include "ControllerIndices.h"

#include <glm/glm.hpp>

namespace TerranEngine {

	class Input 
	{
	public:
		static bool IsKeyPressed(Key key);

		static bool IsMouseButtonPressed(MouseButton button);

		static void SetCursorState(CursorState cursorState);

		static glm::vec2 GetMousePos();

		static bool IsControllerConnected(ControllerIndex controllerIndex);
		static const char* GetControllerName(ControllerIndex controllerIndex);

		static bool IsControllerButtonPressed(ControllerButton controllerButton, ControllerIndex controllerIndex);
		static float GetControllerAxis(ControllerAxis controllerAxis, ControllerIndex controllerIndex);

		static glm::vec2 GetControllerRightStick(ControllerIndex controllerIndex);
		static glm::vec2 GetControllerLeftStick(ControllerIndex controllerIndex);

		static float GetMouseX() { return GetMousePos().x; }
		static float GetMouseY() { return GetMousePos().y; }
	};
}
