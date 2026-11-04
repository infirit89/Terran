namespace Terran
{
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
	}
	
	public enum ControllerAxis : byte
	{
		LeftX			= 0,
		LeftY			= 1,
		RightX			= 2,
		RightY			= 3,
		LeftTrigger		= 4,
		RightTrigger	= 5
	}
}
