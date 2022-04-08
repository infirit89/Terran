using System;
using System.Runtime.CompilerServices;

namespace TerranScriptCore
{
    public class Internal
    {
		public class Log 
		{
			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void Log_Internal(byte level, string text);
		}

		public class Input 
		{
			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern bool KeyPressed_Internal(UInt32 keyCode);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern bool MouseButtonPressed_Internal(UInt16 mouseButton);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void GetMousePosition_Internal(out Vector2 outMousePosition);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern bool IsControllerConnected_Internal(byte controllerIndex);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern string GetControllerName_Internal(byte controllerIndex);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern bool IsControllerButtonPressed_Internal(byte controllerButton, byte controllerIndex);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern float GetControllerAxis_Internal(byte controllerAxis, byte controllerIndex);
		}

		public class Entity 
		{
			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern bool HasComponent_Internal(byte[] runtimeID, string componentTypeStr);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void AddComponent_Internal(byte[] runtimeID, string componentTypeStr);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void RemoveComponent_Internal(byte[] runtimeID, string componentTypeStr);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern object GetScriptableComponent_Internal(byte[] uuid, string moduleName);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern byte[] FindEntityWithName_Internal(string name);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern byte[] FindEntityWithID_Internal(UUID id);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void DestroyEntity_Internal(byte[] id);
		}

		public class Rigidbody2D 
		{
			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern bool IsFixedRotation_Internal(byte[] entityUUID);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void SetFixedRotation_Internal(byte[] entityUUID, bool fixedRotation);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern byte GetAwakeState_Internal(byte[] entityUUID);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void SetAwakeState_Internal(byte[] entityUUID, byte awakeState);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern float GetGravityScale_Internal(byte[] entityUUID);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void SetGravityScale_Internal(byte[] entityUUID, float gravityScale);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void ApplyForce_Internal(byte[] entityUUID, in Vector2 force, in Vector2 position, byte forceMode);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void ApplyForceAtCenter_Internal(byte[] entityUUID, in Vector2 force, byte forceMode);
		}

		public class BoxCollider2D
		{
			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void GetSize_Internal(byte[] entityID, out Vector2 outSize);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void SetSize_Internal(byte[] entityID, in Vector2 size);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void GetOffset_Internal(byte[] entityID, out Vector2 outOffset);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void SetOffset_Internal(byte[] entityUUID, in Vector2 offset);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern bool IsSensor_Internal(byte[] entityUUID);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void SetSensor_Internal(byte[] entityUUID, bool isSensor);
		}

		public class CircleCollider2D 
		{
			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern float GetRadius_Internal(byte[] entityID);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void SetRadius_Internal(byte[] entityID, float radius);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void GetOffset_Internal(byte[] entityID, out Vector2 outVec);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void SetOffset_Internal(byte[] entityUUID, in Vector2 offset);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern bool IsSensor_Internal(byte[] entityUUID);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void SetSensor_Internal(byte[] entityUUID, bool isSensor);
		}

		public class Tag 
		{
			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void SetName_Internal(byte[] entityID, in string inName);
			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern string GetName_Internal(byte[] entityID);
		}

		public class Transform 
		{
			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern Vector3 GetPosition_Internal(byte[] entityID);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void SetPosition_Internal(byte[] entityID, in Vector3 inPosition);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern Vector3 GetRotation_Internal(byte[] entityID);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void SetRotation_Internal(byte[] entityID, in Vector3 inRotation);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern Vector3 GetScale_Internal(byte[] entityID);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern void SetScale_Internal(byte[] entityID, in Vector3 inScale);

			[MethodImpl(MethodImplOptions.InternalCall)]
			public static extern bool IsDirty_Internal(byte[] entityID);

		}
	}
}
