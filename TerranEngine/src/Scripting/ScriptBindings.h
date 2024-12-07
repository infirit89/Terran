#pragma once

#include "Core/KeyCodes.h"
#include "Core/MouseButtons.h"
#include "Core/ControllerIndices.h"

#include "Physics/PhysicsStates.h"
#include "Physics/Collider.h"

#include <Coral/String.hpp>
#include <Coral/ManagedObject.hpp>
#include <glm/glm.hpp>

namespace TerranEngine 
{
	class ScriptBindings final
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
		static void Input_KeyPressedICall(Key keyCode, bool& isPressed);
		static void Input_KeyDownICall(Key keyCode, bool& isDown);
		static void Input_KeyReleasedICall(Key keyCode, bool& isReleased);

		static bool Input_MouseButtonPressedICall(MouseButton mouseButton);
		static bool Input_MouseButtonDownICall(MouseButton mouseButton);
		static bool Input_MouseButtonReleasedICall(MouseButton mouseButton);
		static void Input_GetMousePositionICall(glm::vec2& outMousePosition);

		static bool Input_IsControllerConnectedICall(uint8_t controllerIndex);
		static Coral::String Input_GetControllerNameICall(uint8_t controllerIndex);
		static bool Input_IsControllerButtonPressedICall(uint8_t controllerIndex, ControllerButton controllerButton);
		static float Input_GetControllerAxisICall(uint8_t controllerIndex, ControllerAxis controllerAxis);
		static void* Input_GetConnectedControllersICall();
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
		
		static int Entity_GetChildrenCountICall(const UUID& id);
		static UUID Entity_GetChildICall(const UUID& id, int index);
		static UUID Entity_GetParentICall(const UUID& id);
		#pragma endregion
		// ----------------

		// ---- Physics ----
		static Coral::String LayerMask_GetNameICall(uint16_t layer);


		// ---- Physics 2D ----
		#pragma region Physics 2D
		struct ScopedInstance
		{
			ScopedInstance(Coral::ManagedObject object)
				: Object(object)
			{}

			~ScopedInstance()
			{
				Object.Destroy();
			}

			Coral::ManagedObject Object;
		};


		struct RayCastHitInfo2D_Internal
		{
			glm::vec2 Point = { 0.0f, 0.0f };
			glm::vec2 Normal = { 0.0f, 0.0f };
			UUID RigidbodyEntityId;
		};

		static bool Physics2D_RayCastICall(const glm::vec2& origin, const glm::vec2& direction, float length, uint16_t layerMask, RayCastHitInfo2D_Internal& outHitInfo);
		static void* Physics2D_RayCastAllICall(const glm::vec2& origin, const glm::vec2& direction, float length, uint16_t layerMask);
		#pragma endregion
		// --------------------
		

		// ---- Rigidbody 2D ----
		#pragma region Rigidbody 2D
		static bool Rigidbody2D_IsFixedRotationICall(const UUID& id);
		static void Rigidbody2D_SetFixedRotationICall(const UUID& id, bool fixedRotation);

		static PhysicsBodySleepState Rigidbody2D_GetSleepStateICall(const UUID& id);
		static void Rigidbody2D_SetSleepStateICall(const UUID& id, PhysicsBodySleepState sleepState);

		static float Rigidbody2D_GetGravityScaleICall(const UUID& id);
		static void Rigidbody2D_SetGravityScaleICall(const UUID& id, float gravityScale);

		static void Rigidbody2D_ApplyForceICall(const UUID& id, const glm::vec2& force, const glm::vec2& position, ForceMode2D forceMode);
		static void Rigidbody2D_ApplyForceAtCenterICall(const UUID& id, const glm::vec2& force, ForceMode2D forceMode);

		static glm::vec2 Rigidbody2D_GetLinearVelocityICall(const UUID& id);
		static void Rigidbody2D_SetLinearVelocityICall(const UUID& id, const glm::vec2& linearVelocity);

		static float Rigidbody2D_GetAngularVelocityICall(const UUID& id);
		static void Rigidbody2D_SetAngularVelocityICall(const UUID& id, float angularVelocity);

		static PhysicsBodyType Rigidbody2D_GetTypeICall(const UUID& id);
		static void Rigidbody2D_SetTypeICall(const UUID& id, PhysicsBodyType bodyType);
		#pragma endregion
		// ----------------------

		// ---- Collider 2D ----
		#pragma region Collider 2D
		static void Collider2D_GetOffsetICall(const UUID& id, ColliderType2D colliderType, glm::vec2& offset);
		static void Collider2D_SetOffsetICall(const UUID& id, ColliderType2D colliderType, const glm::vec2& inOffset);

		static void Collider2D_IsSensorICall(const UUID& id, ColliderType2D colliderType, bool& sensor);
		static void Collider2D_SetSensorICall(const UUID& id, ColliderType2D colliderType, bool isSensor);
		#pragma endregion
		// ---------------------

		// ---- Box Collider 2D ----
		#pragma region Box Collider 2D
		static void BoxCollider2D_GetSizeICall(const UUID& id, glm::vec2& size);
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
		static void CapsuleCollider2D_GetSizeICall(const UUID& id, glm::vec2& size);
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
		static glm::vec3 Camera_ScreenToWorldPointICall(const UUID& id, const glm::vec3& point);
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

		// ---- Window ----
		#pragma region Window
		static float Window_GetWidthICall();
		static float Window_GetHeightICall();
		static bool Window_IsVSyncICall();
		static glm::vec2 Window_GetContentScaleICall();
		#pragma endregion
		// ----------------

		// ---- Scene ----
		#pragma region Scene
		static UUID Scene_GetMainCameraICall();
		#pragma endregion
		// ---------------

		// ---- Scene Manager ----
		static bool SceneManager_LoadSceneICall(Coral::String scenePath);
		// -----------------------
	};
}
