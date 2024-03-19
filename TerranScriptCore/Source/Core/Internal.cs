using Coral.Managed.Interop;
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Terran
{
	internal static class Internal
	{
		#region Utils
		internal static unsafe delegate* unmanaged<byte, NativeString, void> Log_LogICall = default;
		#endregion

		#region Input

		// ---- Keyboard ----
		internal static unsafe delegate* unmanaged<ushort, bool> Input_KeyPressedICall = default;
		internal static unsafe delegate* unmanaged<ushort, bool> Input_KeyDownICall = default;
		internal static unsafe delegate* unmanaged<ushort, bool> Input_KeyReleasedICall = default;
		// ------------------

		// ---- Mouse ----
		internal static unsafe delegate* unmanaged<byte, bool> Input_MouseButtonPressedICall = default;
		internal static unsafe delegate* unmanaged<byte, bool> Input_MouseButtonDownICall = default;
		internal static unsafe delegate* unmanaged<byte, bool> Input_MouseButtonReleasedICall = default;
		internal static unsafe delegate* unmanaged<out Vector2, void> Input_GetMousePositionICall = default;
		// ---------------

		// ---- Controller ----
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsControllerConnected(byte controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern string Input_GetControllerName(byte controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsControllerButtonPressed(byte controllerButton, byte controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Input_GetControllerAxis(byte controllerAxis, byte controllerIndex);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern byte[] Input_GetConnectedControllers();
		// --------------------
		#endregion

		#region Entity
		internal static unsafe delegate* unmanaged<in UUID, int, bool> Entity_HasComponentICall = default;
		internal static unsafe delegate* unmanaged<in UUID, int, void> Entity_AddComponentICall = default;		
		internal static unsafe delegate* unmanaged<in UUID, int, void> Entity_RemoveComponentICall = default;
		internal static unsafe delegate* unmanaged<in UUID, IntPtr> Entity_GetScriptableComponentICall = default;
		internal static unsafe delegate* unmanaged<NativeString, out UUID, bool> Entity_FindEntityWithNameICall = default;
		internal static unsafe delegate* unmanaged<in UUID, void> Entity_DestroyEntityICall = default;
		internal static unsafe delegate* unmanaged<in UUID, IntPtr> Entity_GetChildrenICall = default;
		#endregion

		#region Physics 2D
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Physics2D_RayCast(in Vector2 origin, in Vector2 direction, float length, out RayCastHitInfo2D hitInfo, ushort layerMask);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern RayCastHitInfo2D[] Physics2D_RayCastAll(in Vector2 origin, in Vector2 direction, float length, ushort layerMask);
		#endregion

		#region Physics Body 2D
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Rigidbody2D_IsFixedRotation(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_SetFixedRotation(byte[] entityUUID, bool fixedRotation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern byte Rigidbody2D_GetSleepState(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_SetSleepState(byte[] entityUUID, byte awakeState);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Rigidbody2D_GetGravityScale(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_SetGravityScale(byte[] entityUUID, float gravityScale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_ApplyForce(byte[] entityUUID, in Vector2 force, in Vector2 position, byte forceMode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_ApplyForceAtCenter(byte[] entityUUID, in Vector2 force, byte forceMode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_GetLinearVelocity(byte[] entityUUID, out Vector2 linearVelocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_SetLinearVelocity(byte[] entityUUID, in Vector2 linearVelocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Rigidbody2D_GetAngularVelocity(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_SetAngularVelocity(byte[] entityUUID, float angularVelocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern byte Rigidbody2D_GetType(byte[] entityUUID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2D_SetType(byte[] entityUUID, byte bodyType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string LayerMask_GetName(ushort layer);
        #endregion

        #region Collider 2D
        [MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Collider2D_GetOffset(byte[] entityUUID, byte colliderType, out Vector2 offset);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Collider2D_SetOffset(byte[] entityUUID, byte colliderType, in Vector2 offset);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Collider2D_IsSensor(byte[] entityUUID, byte colliderType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Collider2D_SetSensor(byte[] entityUUID, byte colliderType, bool isSensor);
        #endregion

        #region Box Collider 2D
        [MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void BoxCollider2D_GetSize(byte[] entityID, out Vector2 outSize);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void BoxCollider2D_SetSize(byte[] entityID, in Vector2 size);
        #endregion

        #region Circle Collider 2D
        [MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float CircleCollider2D_GetRadius(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void CircleCollider2D_SetRadius(byte[] entityID, float radius);
		#endregion

		#region Capsule Collider 2D
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void CapsuleCollider2D_GetSize(byte[] entityID, out Vector2 size);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void CapsuleCollider2D_SetSize(byte[] entityID, in Vector2 size);
		#endregion

		#region Tag
		internal static unsafe delegate* unmanaged<in UUID, NativeString> Tag_GetNameICall = default;
		internal static unsafe delegate* unmanaged<in UUID, NativeString, void> Tag_SetNameICall = default;
        #endregion

        #region Transform
		internal static unsafe delegate* unmanaged<in UUID, Vector3> Transform_GetPositionICall = default;
		internal static unsafe delegate* unmanaged<in UUID, in Vector3, void> Transform_SetPositionICall = default;
		internal static unsafe delegate* unmanaged<in UUID, Vector3> Transform_GetRotationICall = default;
		internal static unsafe delegate* unmanaged<in UUID, in Vector3, void> Transform_SetRotationICall = default;
		internal static unsafe delegate* unmanaged<in UUID, Vector3> Transform_GetScaleICall = default;
		internal static unsafe delegate* unmanaged<in UUID, in Vector3, void> Transform_SetScaleICall = default;
		internal static unsafe delegate* unmanaged<in UUID, bool> Transform_IsDirtyICall = default;
		internal static unsafe delegate* unmanaged<in UUID, Vector3> Transform_GetForwardICall = default;
		internal static unsafe delegate* unmanaged<in UUID, Vector3> Transform_GetUpICall = default;
		internal static unsafe delegate* unmanaged<in UUID, Vector3> Transform_GetRightICall = default;
        #endregion

        #region SpriteRender
        internal static unsafe delegate* unmanaged<in UUID, Color> SpriteRenderer_GetColorICall = default;
        internal static unsafe delegate* unmanaged<in UUID, in Color, void> SpriteRenderer_SetColorICall = default;
        #endregion

        #region Camera
		
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Camera_IsPrimary(byte[] entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Camera_SetPrimary(byte[] entityID, bool togglePrimary);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Color Camera_GetBackgroundColor(byte[] entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Camera_SetBackgroundColor(byte[] entityID, in Color color);
        #endregion

        #region CircleRenderer
		
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float CircleRenderer_GetThickness(byte[] entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void CircleRenderer_SetThickness(byte[] entityID, float thickness);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Color CircleRenderer_GetColor(byte[] entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void CircleRenderer_SetColor(byte[] entityID, in Color color);
		#endregion

		#region TextRenderer

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern Color TextRenderer_GetColor(byte[] entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void TextRenderer_SetColor(byte[] entityID, in Color color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string TextRenderer_GetText(byte[] entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void TextRenderer_SetText(byte[] entityID, string text);
        #endregion
    }
}

