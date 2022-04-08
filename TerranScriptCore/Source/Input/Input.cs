using System;
using System.Runtime.CompilerServices;

namespace TerranScriptCore
{
	public class Input
	{

		public static bool IsKeyPressed(KeyCode keyCode) => Internal.Input.KeyPressed_Internal((UInt32)keyCode);

		public static bool IsMouseButtonPressed(MouseButton mouseButton) => Internal.Input.MouseButtonPressed_Internal((UInt16)mouseButton);

		public static Vector2 GetMousePosition() 
		{
			Vector2 mousePosition;
			Internal.Input.GetMousePosition_Internal(out mousePosition);
			return mousePosition;
		}

		public static float GetMouseX() => GetMousePosition().X;
		public static float GetMouseY() => GetMousePosition().Y;

		public static bool IsControllerConnected(byte controllerIndex) => Internal.Input.IsControllerConnected_Internal(controllerIndex);

		public static string GetControllerName(byte controllerIndex) => Internal.Input.GetControllerName_Internal(controllerIndex);

		public static bool IsControllerButtonPressed(ControllerButton controllerButton, byte controllerIndex) => Internal.Input.IsControllerButtonPressed_Internal((byte)controllerButton, controllerIndex);

		public static float GetControllerAxis(ControllerAxis controllerAxis, byte controllerIndex) => Internal.Input.GetControllerAxis_Internal((byte)controllerAxis, controllerIndex);
	}
}
