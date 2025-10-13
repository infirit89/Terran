#pragma once

#include "Core/ControllerIndices.h"
#include "Core/KeyCodes.h"
#include "Core/MouseButtons.h"

#include "Physics/Collider.h"
#include "Physics/PhysicsStates.h"

#include <Coral/ManagedObject.hpp>
#include <Coral/String.hpp>
#include <glm/glm.hpp>

namespace TerranEngine {

class ScriptBindings final {
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
    static bool Entity_HasComponentICall(const Terran::Core::UUID& id, int32_t typeId);
    static void Entity_AddComponentICall(Terran::Core::UUID const& id, int32_t typeId);
    static void Entity_RemoveComponentICall(Terran::Core::UUID const& id, int32_t typeId);
    static void* Entity_GetScriptableComponentICall(Terran::Core::UUID const& id);

    static bool Entity_FindEntityWithNameICall(Coral::String entityName, Terran::Core::UUID& id);

    static void Entity_DestroyEntityICall(Terran::Core::UUID const& id);

    static int Entity_GetChildrenCountICall(Terran::Core::UUID const& id);
    static Terran::Core::UUID Entity_GetChildICall(Terran::Core::UUID const& id, int index);
    static Terran::Core::UUID Entity_GetParentICall(Terran::Core::UUID const& id);
#pragma endregion
    // ----------------

    // ---- Physics ----
    static Coral::String LayerMask_GetNameICall(uint16_t layer);

// ---- Physics 2D ----
#pragma region Physics 2D
    struct ScopedInstance {
        ScopedInstance(Coral::ManagedObject object)
            : Object(object)
        {
        }

        ~ScopedInstance()
        {
            Object.Destroy();
        }

        Coral::ManagedObject Object;
    };

    struct RayCastHitInfo2D_Internal {
        glm::vec2 Point = { 0.0f, 0.0f };
        glm::vec2 Normal = { 0.0f, 0.0f };
        Terran::Core::UUID RigidbodyEntityId;
    };

    static bool Physics2D_RayCastICall(glm::vec2 const& origin, glm::vec2 const& direction, float length, uint16_t layerMask, RayCastHitInfo2D_Internal& outHitInfo);
    static void* Physics2D_RayCastAllICall(glm::vec2 const& origin, glm::vec2 const& direction, float length, uint16_t layerMask);
#pragma endregion
// --------------------

// ---- Rigidbody 2D ----
#pragma region Rigidbody 2D
    static bool Rigidbody2D_IsFixedRotationICall(const Terran::Core::UUID& id);
    static void Rigidbody2D_SetFixedRotationICall(Terran::Core::UUID const& id, bool fixedRotation);

    static PhysicsBodySleepState Rigidbody2D_GetSleepStateICall(Terran::Core::UUID const& id);
    static void Rigidbody2D_SetSleepStateICall(Terran::Core::UUID const& id, PhysicsBodySleepState sleepState);

    static float Rigidbody2D_GetGravityScaleICall(Terran::Core::UUID const& id);
    static void Rigidbody2D_SetGravityScaleICall(Terran::Core::UUID const& id, float gravityScale);

    static void Rigidbody2D_ApplyForceICall(Terran::Core::UUID const& id, glm::vec2 const& force, glm::vec2 const& position, ForceMode2D forceMode);
    static void Rigidbody2D_ApplyForceAtCenterICall(Terran::Core::UUID const& id, glm::vec2 const& force, ForceMode2D forceMode);

    static glm::vec2 Rigidbody2D_GetLinearVelocityICall(Terran::Core::UUID const& id);
    static void Rigidbody2D_SetLinearVelocityICall(Terran::Core::UUID const& id, glm::vec2 const& linearVelocity);

    static float Rigidbody2D_GetAngularVelocityICall(Terran::Core::UUID const& id);
    static void Rigidbody2D_SetAngularVelocityICall(Terran::Core::UUID const& id, float angularVelocity);

    static PhysicsBodyType Rigidbody2D_GetTypeICall(Terran::Core::UUID const& id);
    static void Rigidbody2D_SetTypeICall(Terran::Core::UUID const& id, PhysicsBodyType bodyType);
#pragma endregion
// ----------------------

// ---- Collider 2D ----
#pragma region Collider 2D
    static void Collider2D_GetOffsetICall(const Terran::Core::UUID& id, ColliderType2D colliderType, glm::vec2& offset);
    static void Collider2D_SetOffsetICall(Terran::Core::UUID const& id, ColliderType2D colliderType, glm::vec2 const& inOffset);

    static void Collider2D_IsSensorICall(Terran::Core::UUID const& id, ColliderType2D colliderType, bool& sensor);
    static void Collider2D_SetSensorICall(Terran::Core::UUID const& id, ColliderType2D colliderType, bool isSensor);
#pragma endregion
// ---------------------

// ---- Box Collider 2D ----
#pragma region Box Collider 2D
    static void BoxCollider2D_GetSizeICall(const Terran::Core::UUID& id, glm::vec2& size);
    static void BoxCollider2D_SetSizeICall(Terran::Core::UUID const& id, glm::vec2 const& size);
#pragma endregion
// -------------------------

// ---- Circle Collider 2D ----
#pragma region Circle Collider 2D
    static float CircleCollider2D_GetRadiusICall(const Terran::Core::UUID& id);
    static void CircleCollider2D_SetRadiusICall(Terran::Core::UUID const& id, float radius);
#pragma endregion
// ----------------------------

// ---- Capsule Collider 2D ----
#pragma region Capsule Collider 2D
    static void CapsuleCollider2D_GetSizeICall(const Terran::Core::UUID& id, glm::vec2& size);
    static void CapsuleCollider2D_SetSizeICall(Terran::Core::UUID const& id, glm::vec2 const& size);
#pragma endregion
// -----------------------------

// ------------------

// ---- Tag component ----
#pragma region Tag Component
    static Coral::String Tag_GetNameICall(const Terran::Core::UUID& id);
    static void Tag_SetNameICall(Terran::Core::UUID const& id, Coral::String name);
#pragma endregion
// -----------------------

// ---- Transform component ----
#pragma region Transform Component
    static glm::vec3 Transform_GetPositionICall(const Terran::Core::UUID& id);
    static void Transform_SetPositionICall(Terran::Core::UUID const& id, glm::vec3 const& inPosition);

    static glm::vec3 Transform_GetRotationICall(Terran::Core::UUID const& id);
    static void Transform_SetRotationICall(Terran::Core::UUID const& id, glm::vec3 const& inRotation);

    static glm::vec3 Transform_GetScaleICall(Terran::Core::UUID const& id);
    static void Transform_SetScaleICall(Terran::Core::UUID const& id, glm::vec3 const& inScale);

    static bool Transform_IsDirtyICall(Terran::Core::UUID const& id);

    static glm::vec3 Transform_GetForwardICall(Terran::Core::UUID const& id);
    static glm::vec3 Transform_GetUpICall(Terran::Core::UUID const& id);
    static glm::vec3 Transform_GetRightICall(Terran::Core::UUID const& id);
#pragma endregion
// -----------------------------

// ---- Sprite Renderer Component ----
#pragma region Sprite Renderer Component
    static glm::vec4 SpriteRenderer_GetColorICall(const Terran::Core::UUID& id);
    static void SpriteRenderer_SetColorICall(Terran::Core::UUID const& id, glm::vec4 const& color);
#pragma endregion
// -----------------------------------

// ---- Camera Component ----
#pragma region Camera Component
    static bool Camera_IsPrimaryICall(const Terran::Core::UUID& id);
    static void Camera_SetPrimaryICall(Terran::Core::UUID const& id, bool togglePrimary);
    static glm::vec4 Camera_GetBackgroundColorICall(Terran::Core::UUID const& id);
    static void Camera_SetBackgroundColorICall(Terran::Core::UUID const& id, glm::vec4 const& color);
    static glm::vec3 Camera_ScreenToWorldPointICall(Terran::Core::UUID const& id, glm::vec3 const& point);
#pragma endregion
// --------------------------

// ---- Circle Renderer Component ----
#pragma region Circle Renderer Component
    static glm::vec4 CircleRenderer_GetColorICall(const Terran::Core::UUID& id);
    static void CircleRenderer_SetColorICall(Terran::Core::UUID const& id, glm::vec4 const& color);

    static float CircleRenderer_GetThicknessICall(Terran::Core::UUID const& id);
    static void CircleRenderer_SetThicknessICall(Terran::Core::UUID const& id, float thickness);
#pragma endregion
// -----------------------------------

// ---- Text Renderer Component ----
#pragma region Text Renderer Component
    static glm::vec4 TextRenderer_GetColorICall(const Terran::Core::UUID& id);
    static void TextRenderer_SetColorICall(Terran::Core::UUID const& id, glm::vec4 const& color);

    static Coral::String TextRenderer_GetTextICall(Terran::Core::UUID const& id);
    static void TextRenderer_SetTextICall(Terran::Core::UUID const& id, Coral::String text);
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
    static Terran::Core::UUID Scene_GetMainCameraICall();
#pragma endregion
    // ---------------

    // ---- Scene Manager ----
    static bool SceneManager_LoadSceneICall(Coral::String scenePath);
    // -----------------------
};

}
