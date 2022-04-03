#pragma once

#include "KeyCodes.h"
#include "MouseButtons.h"
#include "CursorStates.h"

#include <glm/glm.hpp>

namespace TerranEngine {

	class Input 
	{
	public:
		static bool IsKeyPressed(Key key);

		static bool IsMouseButtonPressed(MouseButton button);

		static void SetCursorState(CursorState cursorState);

		static glm::vec2 GetMousePos();

		static float GetMouseX() { return GetMousePos().x; }
		static float GetMouseY() { return GetMousePos().y; }
	};
}