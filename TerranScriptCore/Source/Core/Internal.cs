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
		[MethodImplICall = default]
		internal static extern bool Input_IsControllerConnectedICall = default;

		[MethodImplICall = default]
		internal static extern string Input_GetControllerNameICall = default;

		[MethodImplICall = default]
		internal static extern bool Input_IsControllerButtonPressedICall = default;

		[MethodImplICall = default]
		internal static extern float Input_GetControllerAxisICall = default;

		[MethodImplICall = default]
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
		[MethodImplICall = default]
		internal static extern bool Physics2D_RayCastICall = default;

		[MethodImplICall = default]
		internal static extern RayCastHitInfo2D[] Physics2D_RayCastAllICall = default;
		#endregion

		#region Physics Body 2D
		[MethodImplICall = default]
		internal static extern bool Rigidbody2D_IsFixedRotationICall = default;

		[MethodImplICall = default]
		internal static extern void Rigidbody2D_SetFixedRotationICall = default;

		[MethodImplICall = default]
		internal static extern byte Rigidbody2D_GetSleepStateICall = default;

		[MethodImplICall = default]
		internal static extern void Rigidbody2D_SetSleepStateICall = default;

		[MethodImplICall = default]
		internal static extern float Rigidbody2D_GetGravityScaleICall = default;

		[MethodImplICall = default]
		internal static extern void Rigidbody2D_SetGravityScaleICall = default;

		[MethodImplICall = default]
		internal static extern void Rigidbody2D_ApplyForceICall = default;

		[MethodImplICall = default]
		internal static extern void Rigidbody2D_ApplyForceAtCenterICall = default;

		[MethodImplICall = default]
		internal static extern void Rigidbody2D_GetLinearVelocityICall = default;

		[MethodImplICall = default]
		internal static extern void Rigidbody2D_SetLinearVelocityICall = default;

		[MethodImplICall = default]
		internal static extern float Rigidbody2D_GetAngularVelocityICall = default;

		[MethodImplICall = default]
		internal static extern void Rigidbody2D_SetAngularVelocityICall = default;

		[MethodImplICall = default]
		internal static extern byte Rigidbody2D_GetTypeICall = default;

		[MethodImplICall = default]
		internal static extern void Rigidbody2D_SetTypeICall = default;

        [MethodImplICall = default]
        internal static extern string LayerMask_GetNameICall = default;
        #endregion

        #region Collider 2D
        [MethodImplICall = default]
		internal static extern void Collider2D_GetOffsetICall = default;

		[MethodImplICall = default]
		internal static extern void Collider2D_SetOffsetICall = default;

		[MethodImplICall = default]
		internal static extern bool Collider2D_IsSensorICall = default;

		[MethodImplICall = default]
		internal static extern void Collider2D_SetSensorICall = default;
        #endregion

        #region Box Collider 2D
        [MethodImplICall = default]
		internal static extern void BoxCollider2D_GetSizeICall = default;

		[MethodImplICall = default]
		internal static extern void BoxCollider2D_SetSizeICall = default;
        #endregion

        #region Circle Collider 2D
        [MethodImplICall = default]
		internal static extern float CircleCollider2D_GetRadiusICall = default;

		[MethodImplICall = default]
		internal static extern void CircleCollider2D_SetRadiusICall = default;
		#endregion

		#region Capsule Collider 2D
		[MethodImplICall = default]
		internal static extern void CapsuleCollider2D_GetSizeICall = default;

		[MethodImplICall = default]
		internal static extern void CapsuleCollider2D_SetSizeICall = default;
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
        internal static unsafe delegate* unmanaged <in UUID, bool> Camera_IsPrimaryICall = default;
        internal static unsafe delegate* unmanaged <in UUID, bool, void> Camera_SetPrimaryICall = default;
        internal static unsafe delegate* unmanaged <in UUID, Color> Camera_GetBackgroundColorICall = default;
        internal static unsafe delegate* unmanaged <in UUID, in Color, void> Camera_SetBackgroundColorICall = default;
        #endregion

        #region CircleRenderer
        internal static unsafe delegate* unmanaged<in UUID, float> CircleRenderer_GetThicknessICall = default;
        internal static unsafe delegate* unmanaged<in UUID, float, void> CircleRenderer_SetThicknessICall = default;
        internal static unsafe delegate* unmanaged<in UUID, Color> CircleRenderer_GetColorICall = default;
        internal static unsafe delegate* unmanaged<in UUID, in Color, void> CircleRenderer_SetColorICall = default;
		#endregion

		#region TextRenderer
		internal static unsafe delegate* unmanaged<in UUID, Color> TextRenderer_GetColorICall = default;
        internal static unsafe delegate* unmanaged<in UUID, in Color, void> TextRenderer_SetColorICall = default;
        internal static unsafe delegate* unmanaged<in UUID, NativeString> TextRenderer_GetTextICall = default;
        internal static unsafe delegate* unmanaged<in UUID, NativeString, void> TextRenderer_SetTextICall = default;
        #endregion
    }
}

