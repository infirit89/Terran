using System;

namespace Terran
{
	public class Input
	{

		public static bool IsKeyPressed(KeyCode keyCode) => Internal.Input_KeyPressed((UInt32)keyCode);

		public static bool IsMouseButtonPressed(MouseButton mouseButton) => Internal.Input_MouseButtonPressed((UInt16)mouseButton);

		public static Vector2 GetMousePosition() 
		{
			Vector2 mousePosition;
			Internal.Input_GetMousePosition(out mousePosition);
			return mousePosition;
		}

		public static float GetMouseX() => GetMousePosition().X;
		public static float GetMouseY() => GetMousePosition().Y;

		public static bool IsControllerConnected(byte controllerIndex) => Internal.Input_IsControllerConnected(controllerIndex);

		public static string GetControllerName(byte controllerIndex) => Internal.Input_GetControllerName(controllerIndex);

		public static bool IsControllerButtonPressed(ControllerButton controllerButton, byte controllerIndex) => Internal.Input_IsControllerButtonPressed((byte)controllerButton, controllerIndex);

		public static float GetControllerAxis(ControllerAxis controllerAxis, byte controllerIndex) => Internal.Input_GetControllerAxis((byte)controllerAxis, controllerIndex);
	}
}
