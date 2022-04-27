#pragma once

#include <cstdint>

namespace TerranEngine
{
	enum class ControllerButton : uint8_t 
	{
		ButtonA = 0,
		ButtonB,
		ButtonX,
		ButtonY,
		LeftBumper,
		RightBumper,
		Back,
		Start,
		Guide,
		LeftThumb,
		RightThumb,
		DPadUp,
		DPadRight,
		DPadDown,
		DPadLeft,
		ButtonCross = ButtonA,
		ButtonCircle = ButtonB,
		ButtonSquare = ButtonX,
		ButtonTriangle = ButtonY
	};

	enum class ControllerAxis : uint8_t 
	{
		LeftX = 0,
		LeftY,
		RightX,
		RightY,
		LeftTrigger,
		RightTrigger
	};
}