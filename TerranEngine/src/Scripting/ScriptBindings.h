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
		static Coral::String LayerMask_GetNameICall(uint16_t layer);


		// ---- Physics 2D ----
		#pragma region Physics 2D
		struct RayCastHitInfo2D_Internal
		{
			glm::vec2 Point;
			glm::vec2 Normal;
			MonoObject* UUID;
		};

		static bool Physics2D_RayCastICall(const glm::vec2& origin, const glm::vec2& direction, float length, RayCastHitInfo2D_Internal& outHitInfo, uint16_t layerMask);
		static void* Physics2D_RayCastAllICall(const glm::vec2& origin, const glm::vec2& direction, float length, uint16_t layerMask);
		#pragma endregion
		// --------------------
		

		// ---- Rigidbody 2D ----
		#pragma region Rigidbody 2D
		static bool Rigidbody2D_IsFixedRotationICall(const UUID& id);
		static void Rigidbody2D_SetFixedRotationICall(const UUID& id, bool fixedRotation);

		static uint8_t Rigidbody2D_GetSleepStateICall(const UUID& id);
		static void Rigidbody2D_SetSleepStateICall(const UUID& id, uint8_t awakeState);

		static float Rigidbody2D_GetGravityScaleICall(const UUID& id);
		static void Rigidbody2D_SetGravityScaleICall(const UUID& id, float gravityScale);

		static void Rigidbody2D_ApplyForceICall(const UUID& id, const glm::vec2& force, const glm::vec2& position, uint8_t forceMode);
		static void Rigidbody2D_ApplyForceAtCenterICall(const UUID& id, const glm::vec2& force, uint8_t forceMode);

		static glm::vec2 Rigidbody2D_GetLinearVelocityICall(const UUID& id);
		static void Rigidbody2D_SetLinearVelocityICall(const UUID& id, const glm::vec2& linearVelocity);

		static float Rigidbody2D_GetAngularVelocityICall(const UUID& id);
		static void Rigidbody2D_SetAngularVelocityICall(const UUID& id, float angularVelocity);

		static uint8_t Rigidbody2D_GetTypeICall(const UUID& id);
		static void Rigidbody2D_SetTypeICall(const UUID& id, uint8_t bodyType);
		#pragma endregion
		// ----------------------

		// ---- Collider 2D ----
		#pragma region Collider 2D
		static glm::vec2 Collider2D_GetOffsetICall(const UUID& id, uint8_t colliderType);
		static void Collider2D_SetOffsetICall(const UUID& id, uint8_t colliderType, const glm::vec2& inOffset);

		static bool Collider2D_IsSensorICall(const UUID& id, uint8_t colliderType);
		static void Collider2D_SetSensorICall(const UUID& id, uint8_t colliderType, bool isSensor);
		#pragma endregion
		// ---------------------

		// ---- Box Collider 2D ----
		#pragma region Box Collider 2D
		static glm::vec2 BoxCollider2D_GetSizeICall(const UUID& id);
		static void BoxCollider2D_SetSizeICall(const UUID& id, const glm::vec2& size);
		#pragma endregion
		// -------------------------

		// ---- Circle Collider 2D ----
		#pragma region Circle Collider 2D
		static float CircleCollider2D_GetRadiusICall(const UUID& id);
		static void CircleCollider2D_SetRadiusICall(const UUID& id, float radius);
		#pragma endregion
		// ----------------------------

		// ---- Capsule Collider 2D ----
		#pragma region Capsule Collider 2D
		static glm::vec2 CapsuleCollider2D_GetSizeICall(const UUID& id);
		static void CapsuleCollider2D_SetSizeICall(const UUID& id, const glm::vec2& size);
		#pragma endregion
		// -----------------------------

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
		#pragma region Circle Renderer Component
		static glm::vec4 CircleRenderer_GetColorICall(const UUID& id);
		static void CircleRenderer_SetColorICall(const UUID& id, const glm::vec4& color);

		static float CircleRenderer_GetThicknessICall(const UUID& id);
		static void CircleRenderer_SetThicknessICall(const UUID& id, float thickness);
		#pragma endregion
		// -----------------------------------

		// ---- Text Renderer Component ----
		#pragma region Text Renderer Component
		static glm::vec4 TextRenderer_GetColorICall(const UUID& id);
		static void TextRenderer_SetColorICall(const UUID& id, const glm::vec4& color);

		static Coral::String TextRenderer_GetTextICall(const UUID& id);
		static void TextRenderer_SetTextICall(const UUID& id, Coral::String text);
		#pragma endregion
		// ---------------------------------
	};
}
