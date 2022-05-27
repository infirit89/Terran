using System;
using System.Runtime.CompilerServices;

namespace Terran
{
    internal class Internal
    {
		// --- Log ---
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Log_Internal(byte level, string text);
		// -----------

		// ---- Input ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_KeyPressed_Internal(UInt32 keyCode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_MouseButtonPressed_Internal(UInt16 mouseButton);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Input_GetMousePosition_Internal(out Vector2 outMousePosition);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsControllerConnected_Internal(byte controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern string Input_GetControllerName_Internal(byte controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsControllerButtonPressed_Internal(byte controllerButton, byte controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Input_GetControllerAxis_Internal(byte controllerAxis, byte controllerIndex);
		// ---------------

		// ---- Entity ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Entity_HasComponent_Internal(byte[] runtimeID, string componentTypeStr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_AddComponent_Internal(byte[] runtimeID, string componentTypeStr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_RemoveComponent_Internal(byte[] runtimeID, string componentTypeStr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern object Entity_GetScriptableComponent_Internal(byte[] uuid, string moduleName);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern byte[] Entity_FindEntityWithName_Internal(string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern byte[] Entity_FindEntityWithID_Internal(byte[] id);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_DestroyEntity_Internal(byte[] id);
		// ----------------

		// ---- Physics 2D ---
		internal struct RayCastHitInfo2D_Internal 
		{
			public Vector2 Point;
			public Vector2 Normal;
			public byte[] UUIDArray;
		}
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Physics2D_RayCast_Internal(in Vector2 origin, in Vector2 direction, float length, out RayCastHitInfo2D_Internal hitInfo);
		// -------------------

		// ---- Rigidbody 2D ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Rigidbody2D_IsFixedRotation_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_SetFixedRotation_Internal(byte[] entityUUID, bool fixedRotation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern byte Rigidbody2D_GetAwakeState_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_SetAwakeState_Internal(byte[] entityUUID, byte awakeState);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Rigidbody2D_GetGravityScale_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_SetGravityScale_Internal(byte[] entityUUID, float gravityScale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_ApplyForce_Internal(byte[] entityUUID, in Vector2 force, in Vector2 position, byte forceMode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_ApplyForceAtCenter_Internal(byte[] entityUUID, in Vector2 force, byte forceMode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_GetLinearVelocity_Internal(byte[] entityUUID, out Vector2 linearVelocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_SetLinearVelocity_Internal(byte[] entityUUID, in Vector2 linearVelocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Rigidbody2D_GetAngularVelocity_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_SetAngularVelocity_Internal(byte[] entityUUID, float angularVelocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern byte Rigidbody2D_GetType_Internal(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_SetType_Internal(byte[] entityUUID, byte bodyType);
		// ----------------------

		// ---- Collider 2D ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Collider2D_GetOffset_Internal(byte[] entityUUID, byte colliderType, out Vector2 offset);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Collider2D_SetOffset_Internal(byte[] entityUUID, byte colliderType, in Vector2 offset);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Collider2D_IsSensor_Internal(byte[] entityUUID, byte colliderType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Collider2D_SetSensor_Internal(byte[] entityUUID, byte colliderType, bool isSensor);


		// ---------------------

		// ---- Box Collider 2D ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void BoxCollider2D_GetSize_Internal(byte[] entityID, out Vector2 outSize);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void BoxCollider2D_SetSize_Internal(byte[] entityID, in Vector2 size);
		// -------------------------
	
		// ---- Circle Collider 2D ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float CircleCollider2D_GetRadius_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void CircleCollider2D_SetRadius_Internal(byte[] entityID, float radius);
		// ----------------------------

		// ---- Tag ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Tag_SetName_Internal(byte[] entityID, in string inName);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern string Tag_GetName_Internal(byte[] entityID);
		// -------------

		// ---- Transform ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern Vector3 Transform_GetPosition_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Transform_SetPosition_Internal(byte[] entityID, in Vector3 inPosition);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern Vector3 Transform_GetRotation_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Transform_SetRotation_Internal(byte[] entityID, in Vector3 inRotation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern Vector3 Transform_GetScale_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Transform_SetScale_Internal(byte[] entityID, in Vector3 inScale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Transform_IsDirty_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern Vector3 Transform_GetForward_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern Vector3 Transform_GetUp_Internal(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern Vector3 Transform_GetRight_Internal(byte[] entityID);


		// -------------------
	}
}
