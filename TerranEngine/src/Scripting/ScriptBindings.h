#pragma once

#include "Core/KeyCodes.h"
#include "Core/MouseButtons.h"
#include "Core/ControllerIndices.h"

#include <Coral/String.hpp>
#include <Coral/Assembly.hpp>
#include <Coral/ManagedObject.hpp>
#include <Coral/ManagedArray.hpp>

#include <glm/glm.hpp>

extern "C" 
{
	typedef struct _MonoString MonoString;
	typedef struct _MonoArray MonoArray;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoReflectionType MonoReflectionType;
}

namespace TerranEngine 
{
	class ScriptBindings
	{
	public:
		static void Bind(Coral::ManagedAssembly& assembly);

		// clear the entity functions
		static void Unbind();

		// ---- Utils ----
		static void Log_LogICall(uint8_t logLevel, Coral::String message);
		// ---------------

		// ---- Input ----
		#pragma region Input
		static bool Input_KeyPressedICall(Key keyCode);
		static bool Input_KeyDownICall(Key keyCode);
		static bool Input_KeyReleasedICall(Key keyCode);

		static bool Input_MouseButtonPressedICall(MouseButton mouseButton);
		static bool Input_MouseButtonDownICall(MouseButton mouseButton);
		static bool Input_MouseButtonReleasedICall(MouseButton mouseButton);
		static void Input_GetMousePositionICall(glm::vec2& outMousePosition);

		static bool Input_IsControllerConnected(uint8_t controllerIndex);
		static MonoString* Input_GetControllerName(uint8_t controllerIndex);
		static bool Input_IsControllerButtonPressed(ControllerButton controllerButton, uint8_t controllerIndex);
		static float Input_GetControllerAxis(ControllerAxis controllerAxis, uint8_t controllerIndex);
		static MonoArray* Input_GetConnectedControllers();
		#pragma endregion
		// ---------------

		// ---- Entity ----
		#pragma region Entity
		static bool Entity_HasComponentICall(const UUID& id, int32_t typeId);
		static void Entity_AddComponentICall(const UUID& id, int32_t typeId);
		static void Entity_RemoveComponentICall(const UUID& id, int32_t typeId);
		static void* Entity_GetScriptableComponentICall(const UUID& id);

		static bool Entity_FindEntityWithNameICall(Coral::String entityName, UUID& id);
		
		static void Entity_DestroyEntityICall(const UUID& id);
		
		static void* Entity_GetChildrenICall(const UUID&);
		#pragma endregion
		// ----------------

		// ---- Physics ----
		// ---- Physics 2D ----
		struct RayCastHitInfo2D_Internal
		{
			glm::vec2 Point;
			glm::vec2 Normal;
			MonoObject* UUID;
		};

		static bool Physics2D_RayCast(const glm::vec2& origin, const glm::vec2& direction, float length, RayCastHitInfo2D_Internal& outHitInfo, uint16_t layerMask);
		static MonoArray* Physics2D_RayCastAll(const glm::vec2& origin, const glm::vec2& direction, float length, uint16_t layerMask);
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
		#pragma region Tag Component
		static Coral::String Tag_GetNameICall(const UUID& id);
		static void Tag_SetNameICall(const UUID& id, Coral::String name);
		#pragma endregion
		// -----------------------

		// ---- Transform component ----
		#pragma region Transform Component
		static glm::vec3 Transform_GetPositionICall(const UUID& id);
		static void Transform_SetPositionICall(const UUID& id, const glm::vec3& inPosition);

		static glm::vec3 Transform_GetRotationICall(const UUID& id);
		static void Transform_SetRotationICall(const UUID& id, const glm::vec3& inRotation);

		static glm::vec3 Transform_GetScaleICall(const UUID& id);
		static void Transform_SetScaleICall(const UUID& id, const glm::vec3& inScale);

		static bool Transform_IsDirtyICall(const UUID& id);

		static glm::vec3 Transform_GetForwardICall(const UUID& id);
		static glm::vec3 Transform_GetUpICall(const UUID& id);
		static glm::vec3 Transform_GetRightICall(const UUID& id);
		#pragma endregion
		// -----------------------------

		// ---- Sprite Renderer Component ----
		#pragma region Sprite Renderer Component
		static glm::vec4 SpriteRenderer_GetColorICall(const UUID& id);
		static void SpriteRenderer_SetColorICall(const UUID& id, const glm::vec4& color);
		#pragma endregion
		// -----------------------------------

		// ---- Camera Component ----
		#pragma region Camera Component
		static bool Camera_IsPrimaryICall(const UUID& id);
		static void Camera_SetPrimaryICall(const UUID& id, bool togglePrimary);
		static glm::vec4 Camera_GetBackgroundColorICall(const UUID& id);
		static void Camera_SetBackgroundColorICall(const UUID& id, const glm::vec4& color);
		#pragma endregion
		// --------------------------

		// ---- Circle Renderer Component ----
		static glm::vec4 CircleRenderer_GetColor(MonoArray* entityUUIDArr);
		static void CircleRenderer_SetColor(MonoArray* entityUUIDArr, const glm::vec4& color);

		static float CircleRenderer_GetThickness(MonoArray* entityUUIDArr);
		static void CircleRenderer_SetThickness(MonoArray* entityUUIDArr, float thickness);
		// -----------------------------------

		// ---- Text Renderer Component ----
		static glm::vec4 TextRenderer_GetColor(MonoArray* entityUUIDArr);
		static void TextRenderer_SetColor(MonoArray* entityUUIDArr, const glm::vec4& color);

		static MonoString* TextRenderer_GetText(MonoArray* entityUUIDArr);
		static void TextRenderer_SetText(MonoArray* entityUUIDArr, MonoString* text);
		// ---------------------------------
	};
}
