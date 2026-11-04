using System;
using System.Runtime.InteropServices;

namespace Terran
{
	public class Input
	{

		public static bool IsKeyPressed(KeyCode keyCode) 
		{
			unsafe
			{
				Internal.Input_KeyPressedICall(keyCode, out bool isPressed);
				return isPressed;
			}
        }
		public static bool IsKeyDown(KeyCode keyCode)
		{
			unsafe 
			{
				Internal.Input_KeyDownICall(keyCode, out bool isDown);
				return isDown;
			}
		}
		public static bool IsKeyReleased(KeyCode keyCode)
		{
			unsafe 
			{
				Internal.Input_KeyReleasedICall(keyCode, out bool isReleased);
				return isReleased;
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
				Internal.Input_GetMousePositionICall(out Vector2 mousePosition);
				return mousePosition;
			}
		}

		public static float GetMouseX() => GetMousePosition().X;
		public static float GetMouseY() => GetMousePosition().Y;

		public static bool IsControllerConnected(byte controllerIndex)
		{
			unsafe 
			{
				return Internal.Input_IsControllerConnectedICall(controllerIndex);
			}
		}

		public static string GetControllerName(byte controllerIndex)
		{
			unsafe 
			{
				return Internal.Input_GetControllerNameICall(controllerIndex)!;
			}
		}

		public static bool IsControllerButtonPressed(byte controllerIndex, ControllerButton controllerButton)
		{
			unsafe 
			{
				return Internal.Input_IsControllerButtonPressedICall(controllerIndex, controllerButton);
			}
		}

		public static float GetControllerAxis(byte controllerIndex, ControllerAxis controllerAxis)
		{
			unsafe 
			{
				return Internal.Input_GetControllerAxisICall(controllerIndex, controllerAxis);
			}
		}

		public static byte[] GetConnectedControllers()
		{
			unsafe 
			{
				IntPtr handle = Internal.Input_GetConnectedControllersICall();
				return GCHandle.FromIntPtr(handle).Target as byte[] ?? throw new NullReferenceException();
			}
		}
	}
}
