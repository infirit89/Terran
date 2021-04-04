#pragma once

#include "KeyCodes.h"
#include "MouseButtons.h"

namespace TerranEngine {

	class Input 
	{
	public:
		static bool IsKeyPressed(uint32_t key);
		static bool IsMouseButtonPressed(uint16_t button);
	};
}