#pragma once

#include "KeyCodes.h"
#include "MouseButtons.h"

namespace TerranEngine {

	class Input 
	{
	public:
		static bool IsKeyPressed(int key);
		static bool IsMouseButtonPressed(int button);
	};
}