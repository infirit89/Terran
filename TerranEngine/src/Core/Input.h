#pragma once

#include "KeyCodes.h"
#include "MouseButtons.h"

namespace TerranEngine {

	class Input 
	{
	public:
		static bool IsKeyPressed(Key key);
		static bool IsMouseButtonPressed(MouseButton button);
	};
}