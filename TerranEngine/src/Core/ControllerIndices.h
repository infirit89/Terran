#pragma once

#include <cstdint>

namespace TerranEngine
{
	enum class ControllerIndex : uint8_t 
	{
		Controller1 = 0,
		Controller2,
		Controller3,
		Controller4,
		Controller5,
		Controller6,
		Controller7,
		Controller8,
		Controller9,
		Controller10,
		Controller11,
		Controller12,
		Controller13,
		Controller14,
		Controller15,
		Controller16,
	};

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