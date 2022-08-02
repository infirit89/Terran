using System;

namespace Terran
{
	public class Input
	{

		public static bool IsKeyPressed(KeyCode keyCode) => Internal.Input_KeyPressed((ushort)keyCode);
		public static bool IsKeyDown(KeyCode keyCode) => Internal.Input_KeyDown((ushort)keyCode);
		public static bool IsKeyReleased(KeyCode keyCode) => Internal.Input_KeyReleased((ushort)keyCode);

		public static bool IsMouseButtonPressed(MouseButton mouseButton) => Internal.Input_MouseButtonPressed((byte)mouseButton);
		public static bool IsMouseButtonDown(MouseButton mouseButton) => Internal.Input_MouseButtonDown((byte)mouseButton);
		public static bool IsMouseButtonReleased(MouseButton mouseButton) => Internal.Input_MouseButtonReleased((byte)mouseButton);

		public static Vector2 GetMousePosition() 
		{
			Internal.Input_GetMousePosition(out var mousePosition);
			return mousePosition;
		}

		public static float GetMouseX() => GetMousePosition().X;
		public static float GetMouseY() => GetMousePosition().Y;

		public static bool IsControllerConnected(byte controllerIndex) => Internal.Input_IsControllerConnected(controllerIndex);

		public static string GetControllerName(byte controllerIndex) => Internal.Input_GetControllerName(controllerIndex);

		public static bool IsControllerButtonPressed(ControllerButton controllerButton, byte controllerIndex) => Internal.Input_IsControllerButtonPressed((byte)controllerButton, controllerIndex);

		public static float GetControllerAxis(ControllerAxis controllerAxis, byte controllerIndex) => Internal.Input_GetControllerAxis((byte)controllerAxis, controllerIndex);

		public static byte[] GetConnectedControllers() => Internal.Input_GetConnectedControllers();
	}
}
