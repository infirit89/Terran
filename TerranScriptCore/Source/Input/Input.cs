using System;

namespace Terran
{
	public class Input
	{

		public static bool IsKeyPressed(KeyCode keyCode) 
		{
			unsafe
			{
				return Internal.Input_KeyPressedICall((ushort)keyCode);
			}
        }
		public static bool IsKeyDown(KeyCode keyCode)
		{
			unsafe 
			{
				return Internal.Input_KeyDownICall((ushort)keyCode);
			}
		}
		public static bool IsKeyReleased(KeyCode keyCode)
		{
			unsafe 
			{
				return Internal.Input_KeyReleasedICall((ushort)keyCode);
			}
		}

		public static bool IsMouseButtonPressed(MouseButton mouseButton)
		{
			unsafe 
			{
				return Internal.Input_MouseButtonPressedICall((byte)mouseButton);
			}
		}
		public static bool IsMouseButtonDown(MouseButton mouseButton)
		{
			unsafe 
			{
				return Internal.Input_MouseButtonDownICall((byte)mouseButton);
			}
		}
		public static bool IsMouseButtonReleased(MouseButton mouseButton)
		{
			unsafe 
			{
				return Internal.Input_MouseButtonReleasedICall((byte)mouseButton);
			}
		}

		public static Vector2 GetMousePosition()
		{
			unsafe 
			{
				Internal.Input_GetMousePositionICall(out Vector2 mousePostion);
				return mousePostion;
			}
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
