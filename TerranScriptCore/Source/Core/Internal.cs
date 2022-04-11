using System;
using System.Runtime.CompilerServices;

namespace TerranScriptCore
{
    public class Internal
    {
		// --- Log ---
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Log_Internal(byte level, string text);
		// -----------

		// ---- Input ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Input_KeyPressed_Internal(UInt32 keyCode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Input_MouseButtonPressed_Internal(UInt16 mouseButton);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Input_GetMousePosition_Internal(out Vector2 outMousePosition);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Input_IsControllerConnected_Internal(byte controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern string Input_GetControllerName_Internal(byte controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Input_IsControllerButtonPressed_Internal(byte controllerButton, byte controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern float Input_GetControllerAxis_Internal(byte controllerAxis, byte controllerIndex);
		// ---------------

		// ---- Entity ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Entity_HasComponent_Internal(byte[] runtimeID, string componentTypeStr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Entity_AddComponent_Internal(byte[] runtimeID, string componentTypeStr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Entity_RemoveComponent_Internal(byte[] runtimeID, string componentTypeStr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern object Entity_GetScriptableComponent_Internal(byte[] uuid, string moduleName);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern byte[] Entity_FindEntityWithName_Internal(string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern byte[] Entity_FindEntityWithID_Internal(UUID id);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Entity_DestroyEntity_Internal(byte[] id);
		// ----------------

		// ---- Rigidbody 2D ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Rigidbody2D_IsFixedRotation_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Rigidbody2D_SetFixedRotation_Internal(byte[] entityUUID, bool fixedRotation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern byte Rigidbody2D_GetAwakeState_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Rigidbody2D_SetAwakeState_Internal(byte[] entityUUID, byte awakeState);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern float Rigidbody2D_GetGravityScale_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Rigidbody2D_SetGravityScale_Internal(byte[] entityUUID, float gravityScale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Rigidbody2D_ApplyForce_Internal(byte[] entityUUID, in Vector2 force, in Vector2 position, byte forceMode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Rigidbody2D_ApplyForceAtCenter_Internal(byte[] entityUUID, in Vector2 force, byte forceMode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Rigidbody2D_GetLinearVelocity_Internal(byte[] entityUUID, out Vector2 linearVelocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Rigidbody2D_SetLinearVelocity_Internal(byte[] entityUUID, in Vector2 linearVelocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern float Rigidbody2D_GetAngularVelocity_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Rigidbody2D_SetAngularVelocity_Internal(byte[] entityUUID, float angularVelocity);
		// ----------------------

		// ---- Box Collider 2D ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void BoxCollider2D_GetSize_Internal(byte[] entityID, out Vector2 outSize);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void BoxCollider2D_SetSize_Internal(byte[] entityID, in Vector2 size);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void BoxCollider2D_GetOffset_Internal(byte[] entityID, out Vector2 outOffset);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void BoxCollider2D_SetOffset_Internal(byte[] entityUUID, in Vector2 offset);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool BoxCollider2D_IsSensor_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void BoxCollider2D_SetSensor_Internal(byte[] entityUUID, bool isSensor);
		// -------------------------
	
		// ---- Circle Collider 2D ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern float CircleCollider2D_GetRadius_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void CircleCollider2D_SetRadius_Internal(byte[] entityID, float radius);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void CircleCollider2D_GetOffset_Internal(byte[] entityID, out Vector2 outVec);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void CircleCollider2D_SetOffset_Internal(byte[] entityUUID, in Vector2 offset);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool CircleCollider2D_IsSensor_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void CircleCollider2D_SetSensor_Internal(byte[] entityUUID, bool isSensor);
		// ----------------------------

		// ---- Tag ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Tag_SetName_Internal(byte[] entityID, in string inName);
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern string Tag_GetName_Internal(byte[] entityID);
		// -------------

		// ---- Transform ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern Vector3 Transform_GetPosition_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Transform_SetPosition_Internal(byte[] entityID, in Vector3 inPosition);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern Vector3 Transform_GetRotation_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Transform_SetRotation_Internal(byte[] entityID, in Vector3 inRotation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern Vector3 Transform_GetScale_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern void Transform_SetScale_Internal(byte[] entityID, in Vector3 inScale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		public static extern bool Transform_IsDirty_Internal(byte[] entityID);

		// -------------------
	}
}
