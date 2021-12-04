#pragma once

#include "KeyCodes.h"
#include "MouseButtons.h"

#include <glm/glm.hpp>

namespace TerranEngine {

	class Input 
	{
	public:
		static bool IsKeyPressed(Key key);
		static bool IsMouseButtonPressed(MouseButton button);

		static glm::vec2 GetMousePos();

		static float GetMouseX() { return GetMousePos().x; }
		static float GetMouseY() { return GetMousePos().y; }
	};
}