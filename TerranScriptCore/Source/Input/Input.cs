using System;
using System.Runtime.CompilerServices;

namespace TerranScriptCore
{
	public class Input
	{

		public static bool IsKeyPressed(KeyCode keyCode) => KeyPressed_Internal((UInt32)keyCode);

		public static bool IsMouseButtonPressed(MouseButton mouseButton) => MouseButtonPressed_Internal((UInt16)mouseButton);

		public static Vector2 GetMousePosition() 
		{
			Vector2 mousePosition;
			GetMousePosition_Internal(out mousePosition);
			return mousePosition;
		}

		public static float GetMouseX() => GetMousePosition().X;
		public static float GetMouseY() => GetMousePosition().Y;

		public static bool IsControllerConnected(byte controllerIndex) => IsControllerConnected_Internal(controllerIndex);

		public static string GetControllerName(byte controllerIndex) => GetControllerName(controllerIndex);

		public static bool IsControllerButtonPressed(ControllerButton controllerButton, byte controllerIndex) => IsControllerButtonPressed_Internal((byte)controllerButton, controllerIndex);

		public static float GetControllerAxis(ControllerAxis controllerAxis, byte controllerIndex) => GetControllerAxis_Internal((byte)controllerAxis, controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern bool KeyPressed_Internal(UInt32 keyCode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern bool MouseButtonPressed_Internal(UInt16 mouseButton);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern void GetMousePosition_Internal(out Vector2 outMousePosition);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern bool IsControllerConnected_Internal(byte controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern string GetControllerName_Internal(byte controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern bool IsControllerButtonPressed_Internal(byte controllerButton, byte controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		static extern float GetControllerAxis_Internal(byte controllerAxis, byte controllerIndex);
	}
}
