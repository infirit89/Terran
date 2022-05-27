using System;

namespace Terran
{
	public enum ControllerIndex : byte
	{
		Controller1		= 0,
		Controller2		= 1,
		Controller3		= 2,
		Controller4		= 3,
		Controller5		= 4,
		Controller6		= 5,
		Controller7		= 6,
		Controller8		= 7,
		Controller9		= 8,
		Controller10	= 9,
		Controller11	= 10,
		Controller12	= 11,
		Controller13	= 12,
		Controller14	= 13,
		Controller15	= 14,
		Controller16	= 15,
	};

	public enum ControllerButton : byte
	{
		ButtonA			= 0,
		ButtonB			= 1,
		ButtonX			= 2,
		ButtonY			= 3,
		LeftBumper		= 4,
		RightBumper		= 5,
		Back			= 6,
		Start			= 7,
		Guide			= 8,
		LeftThumb		= 9,
		RightThumb		= 10,
		DPadUp			= 11,
		DPadRight		= 12,
		DPadDown		= 13,
		DPadLeft		= 14,
		ButtonCross		= ButtonA,
		ButtonCircle	= ButtonB,
		ButtonSquare	= ButtonX,
		ButtonTriangle	= ButtonY
	};
	public enum ControllerAxis : byte
	{
		LeftX			= 0,
		LeftY			= 1,
		RightX			= 2,
		RightY			= 3,
		LeftTrigger		= 4,
		RightTrigger	= 5
	};
}
