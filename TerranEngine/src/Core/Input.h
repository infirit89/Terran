#pragma once

#include "KeyCodes.h"
#include "MouseButtons.h"
#include "CursorStates.h"
#include "ControllerIndices.h"

#include <glm/glm.hpp>

#include <vector>

namespace TerranEngine {

	class Input 
	{
	public:
		static bool IsKeyPressed(Key key);

		static bool IsMouseButtonPressed(MouseButton button);

		static void SetCursorState(CursorState cursorState);

		static glm::vec2 GetMousePos();

		static bool IsControllerConnected(uint8_t controllerIndex);
		static const char* GetControllerName(uint8_t controllerIndex);

		static bool IsControllerButtonPressed(ControllerButton controllerButton, uint8_t controllerIndex);
		static float GetControllerAxis(ControllerAxis controllerAxis, uint8_t controllerIndex);

		static glm::vec2 GetControllerRightStick(uint8_t controllerIndex);
		static glm::vec2 GetControllerLeftStick(uint8_t controllerIndex);

		static std::vector<uint8_t> GetConnectedControllers(); 
		static float GetMouseX() { return GetMousePos().x; }
		static float GetMouseY() { return GetMousePos().y; }
	};
}
