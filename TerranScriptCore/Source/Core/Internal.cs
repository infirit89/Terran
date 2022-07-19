using System;
using System.Runtime.CompilerServices;

namespace Terran
{
    internal class Internal
    {
        #region Log
        [MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Log_Log(byte level, string text);
        #endregion

        #region Input
        [MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_KeyPressed(UInt32 keyCode);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_MouseButtonPressed(UInt16 mouseButton);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Input_GetMousePosition(out Vector2 outMousePosition);

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
		#endregion

        #region Entity
        [MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Entity_HasComponent(byte[] runtimeID, string componentTypeStr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_AddComponent(byte[] runtimeID, string componentTypeStr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_RemoveComponent(byte[] runtimeID, string componentTypeStr);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern object Entity_GetScriptableComponent(byte[] uuid, string moduleName);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern byte[] Entity_FindEntityWithName(string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern byte[] Entity_FindEntityWithID(byte[] id);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_DestroyEntity(byte[] id);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern UUID[] Entity_GetChildren(byte[] id);
		#endregion

        #region Physics 2D
        internal struct RayCastHitInfo2D_Internal 
		{
			public Vector2 Point;
			public Vector2 Normal;
			public byte[] UUIDArray;
		}
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Physics2D_RayCast(in Vector2 origin, in Vector2 direction, float length, out RayCastHitInfo2D_Internal hitInfo);
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

        #region Tag
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern string Tag_GetName(byte[] entityID);
        [MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Tag_SetName(byte[] entityID, in string inName);
        #endregion

        #region Transform
        [MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern Vector3 Transform_GetPosition(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Transform_SetPosition(byte[] entityID, in Vector3 inPosition);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern Vector3 Transform_GetRotation(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Transform_SetRotation(byte[] entityID, in Vector3 inRotation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern Vector3 Transform_GetScale(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Transform_SetScale(byte[] entityID, in Vector3 inScale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Transform_IsDirty(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern Vector3 Transform_GetForward(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern Vector3 Transform_GetUp(byte[] entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern Vector3 Transform_GetRight(byte[] entityID);
        #endregion

        #region SpriteRender
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Color SpriteRenderer_GetColor(byte[] entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SpriteRenderer_SetColor(byte[] entityID, in Color color);
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


    }
}
