#pragma once

#include "Core/KeyCodes.h"
#include "Core/MouseButtons.h"
#include "Core/ControllerIndices.h"

#include <glm/glm.hpp>

extern "C" 
{
	typedef struct _MonoString MonoString;
	typedef struct _MonoArray MonoArray;
	typedef struct _MonoObject MonoObject;
}

namespace TerranEngine 
{
	namespace ScriptBindings
	{
		void Bind();

		// ---- Utils ----
		static void Log_Log(uint8_t logLevel, MonoString* monoMessage);
		// ---------------

		// ---- Input ----
		static bool Input_KeyPressed(Key keyCode);
		static bool Input_KeyDown(Key keyCode);
		static bool Input_KeyReleased(Key keyCode);

		static bool Input_MouseButtonPressed(MouseButton mouseButton);
		static bool Input_MouseButtonDown(MouseButton mouseButton);
		static bool Input_MouseButtonReleased(MouseButton mouseButton);
		static void Input_GetMousePosition(glm::vec2& outMousePosition);

		static bool Input_IsControllerConnected(uint8_t controllerIndex);
		static MonoString* Input_GetControllerName(uint8_t controllerIndex);
		static bool Input_IsControllerButtonPressed(ControllerButton controllerButton, uint8_t controllerIndex);
		static float Input_GetControllerAxis(ControllerAxis controllerAxis, uint8_t controllerIndex);
		static MonoArray* Input_GetConnectedControllers();
		// ---------------

		// ---- Entity ----
		static bool Entity_HasComponent(MonoArray* entityUUIDArr, MonoString* componentType);
		static void Entity_AddComponent(MonoArray* entityUUIDArr, MonoString* componentType);
		static void Entity_RemoveComponent(MonoArray* entityUUIDArr, MonoString* componentType);
		static MonoObject* Entity_GetScriptableComponent(MonoArray* entityUUIDArr, MonoString* moduleName);

		static MonoArray* Entity_FindEntityWithName(MonoString* monoName);
		static MonoArray* Entity_FindEntityWithID(MonoArray* monoIDArray);

		static void Entity_DestroyEntity(MonoArray* entityUUIDArr);
		
		static MonoArray* Entity_GetChildren(MonoArray* entityUUIDArr);
		// ----------------

		// ---- Physics ----
		// ---- Physics 2D ----
		struct RayCastHitInfo2D_Internal
		{
			glm::vec2 Point;
			glm::vec2 Normal;
			MonoArray* UUID;
		};

		static bool Physics2D_RayCast(const glm::vec2& origin, const glm::vec2& direction, float length, RayCastHitInfo2D_Internal& outHitInfo, uint16_t layerMask);
		static int Physics2D_RayCastAll(const glm::vec2& origin, const glm::vec2& direction, float length, uint16_t layerMask);
		// --------------------
		
		// ---- Rigidbody 2D ----
		static bool Rigidbody2D_IsFixedRotation(MonoArray* entityUUIDArr);
		static void Rigidbody2D_SetFixedRotation(MonoArray* entityUUIDArr, bool fixedRotation);

		static uint8_t Rigidbody2D_GetSleepState(MonoArray* entityUUIDArr);
		static void Rigidbody2D_SetSleepState(MonoArray* entityUUIDArr, uint8_t awakeState);

		static float Rigidbody2D_GetGravityScale(MonoArray* entityUUIDArr);
		static void Rigidbody2D_SetGravityScale(MonoArray* entityUUIDArr, float gravityScale);

		static void Rigidbody2D_ApplyForce(MonoArray* entityUUIDArr, const glm::vec2& force, const glm::vec2& position, uint8_t forceMode);
		static void Rigidbody2D_ApplyForceAtCenter(MonoArray* entityUUIDArr, const glm::vec2& force, uint8_t forceMode);

		static void Rigidbody2D_GetLinearVelocity(MonoArray* entityUUIDArr, glm::vec2& linearVelocity);
		static void Rigidbody2D_SetLinearVelocity(MonoArray* entityUUIDArr, const glm::vec2& linearVelocity);

		static float Rigidbody2D_GetAngularVelocity(MonoArray* entityUUIDArr);
		static void Rigidbody2D_SetAngularVelocity(MonoArray* entityUUIDArr, float angularVelocity);

		static uint8_t Rigidbody2D_GetType(MonoArray* entityUUIDArr);
		static void Rigidbody2D_SetType(MonoArray* entityUUIDArr, uint8_t bodyType);
		// ----------------------

		// ---- Collider 2D ----
		static void Collider2D_GetOffset(MonoArray* entityUUIDArr, uint8_t colliderType, glm::vec2& outOffset);
		static void Collider2D_SetOffset(MonoArray* entityUUIDArr, uint8_t colliderType, const glm::vec2& inOffset);

		static bool Collider2D_IsSensor(MonoArray* entityUUIDArr, uint8_t colliderType);
		static void Collider2D_SetSensor(MonoArray* entityUUIDArr, uint8_t colliderType, bool isSensor);
		// ---------------------

		// ---- Box Collider 2D ----
		static void BoxCollider2D_GetSize(MonoArray* entityUUIDArr, glm::vec2& size);
		static void BoxCollider2D_SetSize(MonoArray* entityUUIDArr, const glm::vec2& size);
		// -------------------------

		// ---- Circle Collider 2D ----
		static float CircleCollider2D_GetRadius(MonoArray* entityUUIDArr);
		static void CircleCollider2D_SetRadius(MonoArray* entityUUIDArr, float radius);
		// ----------------------------

		// ---- Capsule Collider 2D ----
		static void CapsuleCollider2D_GetSize(MonoArray* entityUUIDArr, glm::vec2& size);
		static void CapsuleCollider2D_SetSize(MonoArray* entityUUIDArr, const glm::vec2& size);
		// -----------------------------

        // ---- Layer Mask ----
        static MonoString* LayerMask_GetName(uint16_t layer);
        // --------------------

		// ------------------

		// ---- Tag component ----
		static MonoString* Tag_GetName(MonoArray* entityUUIDArr);
		static void Tag_SetName(MonoArray* entityUUIDArr, MonoString* inName);
		// -----------------------

		// ---- Transform component ----
		static glm::vec3 Transform_GetPosition(MonoArray* entityUUIDArr);
		static void Transform_SetPosition(MonoArray* entityUUIDArr, const glm::vec3& inPosition);

		static glm::vec3 Transform_GetRotation(MonoArray* entityUUIDArr);
		static void Transform_SetRotation(MonoArray* entityUUIDArr, const glm::vec3& inRotation);

		static glm::vec3 Transform_GetScale(MonoArray* entityUUIDArr);
		static void Transform_SetScale(MonoArray* entityUUIDArr, const glm::vec3& inScale);

		static bool Transform_IsDirty(MonoArray* entityUUIDArr);

		static glm::vec3 Transform_GetForward(MonoArray* entityUUIDArr);
		static glm::vec3 Transform_GetUp(MonoArray* entityUUIDArr);
		static glm::vec3 Transform_GetRight(MonoArray* entityUUIDArr);
		// -----------------------------

		// ---- Sprite Renderer Component ----
		static glm::vec4 SpriteRenderer_GetColor(MonoArray* entityUUIDArr);
		static void SpriteRenderer_SetColor(MonoArray* entityUUIDArr, const glm::vec4& color);
		// -----------------------------------

		// ---- Camera Component ----
		static bool Camera_IsPrimary(MonoArray* entityUUIDArr);
		static void Camera_SetPrimary(MonoArray* entityUUIDArr, bool togglePrimary);
		static glm::vec4 Camera_GetBackgroundColor(MonoArray* entityUUIDArr);
		static void Camera_SetBackgroundColor(MonoArray* entityUUIDArr,const glm::vec4& color);
		// --------------------------

		// ---- Circle Renderer Component ----
		static glm::vec4 CircleRenderer_GetColor(MonoArray* entityUUIDArr);
		static void CircleRenderer_SetColor(MonoArray* entityUUIDArr, const glm::vec4& color);

		static float CircleRenderer_GetThickness(MonoArray* entityUUIDArr);
		static void CircleRenderer_SetThickness(MonoArray* entityUUIDArr, float thickness);
		// -----------------------------------
	}
}
