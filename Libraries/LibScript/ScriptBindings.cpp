#include "ScriptBindings.h"

#include "ScriptEngine.h"
#include "ScriptTypes.h"

// #include "Core/Application.h"
// #include "Core/Input.h"

#include "ScriptComponent.h"
#include <LibScene/Entity.h>
#include <LibScene/SceneManager.h>
// #include "Scene/Systems/SceneRenderer.h"

// #include "Physics/Collider.h"
// #include "Physics/Physics.h"
// #include "Physics/PhysicsBody.h"
// #include "Physics/PhysicsLayerManager.h"

// #include "Asset/AssetManager.h"

// #include "Utils/Debug/OptickProfiler.h"

#include <glm/glm.hpp>

#include <Coral/Assembly.hpp>
#include <Coral/ManagedArray.hpp>
#include <Coral/Type.hpp>
#include <Coral/TypeCache.hpp>

#include <functional>

namespace Terran::Script {

#define BIND_INTERNAL_FUNC(func) assembly.AddInternalCall("Terran.Internal", #func, reinterpret_cast<void*>(&func))

static std::unordered_map<int32_t, std::function<bool(World::Entity)>> s_HasComponentFuncs;
static std::unordered_map<int32_t, std::function<void(World::Entity)>> s_AddComponentFuncs;
static std::unordered_map<int32_t, std::function<void(World::Entity)>> s_RemoveComponentFuncs;

#define REGISTER_COMPONENT(ComponentType, Component)                                                                                       \
    Coral::Type type_##ComponentType = assembly.GetType("Terran." #ComponentType);                                                         \
    TR_ASSERT(type_##ComponentType, "Invalid type");                                                                                       \
    s_HasComponentFuncs.emplace(type_##ComponentType.GetTypeId(), [](World::Entity entity) { return entity.has_component<Component>(); }); \
    s_AddComponentFuncs.emplace(type_##ComponentType.GetTypeId(), [](World::Entity entity) { entity.add_component<Component>(); });        \
    s_RemoveComponentFuncs.emplace(type_##ComponentType.GetTypeId(), [](World::Entity entity) { entity.remove_component<Component>(); })

void ScriptBindings::Bind(Coral::ManagedAssembly& assembly)
{
//     TR_CORE_INFO(TR_LOG_SCRIPT, "Binding internal calls for core assembly");
//     REGISTER_COMPONENT(Transform, World::TransformComponent);
//     REGISTER_COMPONENT(Tag, World::TagComponent);
//
//     BIND_INTERNAL_FUNC(Log_LogICall);
//
// // ---- Entity ----
// #pragma region Entity
//     BIND_INTERNAL_FUNC(Entity_HasComponentICall);
//     BIND_INTERNAL_FUNC(Entity_AddComponentICall);
//     BIND_INTERNAL_FUNC(Entity_RemoveComponentICall);
//     BIND_INTERNAL_FUNC(Entity_GetScriptableComponentICall);
//     BIND_INTERNAL_FUNC(Entity_DestroyEntityICall);
//     BIND_INTERNAL_FUNC(Entity_FindEntityWithNameICall);
//     BIND_INTERNAL_FUNC(Entity_GetChildrenCountICall);
//     BIND_INTERNAL_FUNC(Entity_GetChildICall);
//     BIND_INTERNAL_FUNC(Entity_GetParentICall);
// #pragma endregion
// // ----------------
//
// // ---- Tag Component ----
// #pragma region Tag Component
//     BIND_INTERNAL_FUNC(Tag_GetNameICall);
//     BIND_INTERNAL_FUNC(Tag_SetNameICall);
// #pragma endregion
// // -------------
//
// // ---- Transform Component ----
// #pragma region Transform Component
//     BIND_INTERNAL_FUNC(Transform_GetPositionICall);
//     BIND_INTERNAL_FUNC(Transform_SetPositionICall);
//     BIND_INTERNAL_FUNC(Transform_GetRotationICall);
//     BIND_INTERNAL_FUNC(Transform_SetRotationICall);
//     BIND_INTERNAL_FUNC(Transform_GetScaleICall);
//     BIND_INTERNAL_FUNC(Transform_SetScaleICall);
//     BIND_INTERNAL_FUNC(Transform_IsDirtyICall);
//     BIND_INTERNAL_FUNC(Transform_GetForwardICall);
//     BIND_INTERNAL_FUNC(Transform_GetUpICall);
//     BIND_INTERNAL_FUNC(Transform_GetRightICall);
// #pragma endregion
//     // -----------------------------
//     // ---- Window ----
//     // #pragma region Window
//     //     BIND_INTERNAL_FUNC(Window_GetWidthICall);
//     //     BIND_INTERNAL_FUNC(Window_GetHeightICall);
//     //     BIND_INTERNAL_FUNC(Window_IsVSyncICall);
//     //     BIND_INTERNAL_FUNC(Window_GetContentScaleICall);
//     // #pragma endregion
//     // ----------------
//
//     BIND_INTERNAL_FUNC(SceneManager_LoadSceneICall);
//
//     assembly.UploadInternalCalls();
}

void ScriptBindings::Unbind()
{
    s_HasComponentFuncs.clear();
    s_AddComponentFuncs.clear();
    s_RemoveComponentFuncs.clear();
}


// ---- Input ----
// #pragma region Input
// void ScriptBindings::Input_KeyPressedICall(Key keyCode, bool& isPressed)
// {
//     isPressed = Input::IsKeyPressed(keyCode);
// }
//
// void ScriptBindings::Input_KeyDownICall(Key keyCode, bool& isDown)
// {
//     isDown = Input::IsKeyDown(keyCode);
// }
//
// void ScriptBindings::Input_KeyReleasedICall(Key keyCode, bool& isReleased)
// {
//     isReleased = Input::IsKeyReleased(keyCode);
// }
//
// bool ScriptBindings::Input_MouseButtonPressedICall(MouseButton mouseButton)
// {
//     return Input::IsMouseButtonPressed(mouseButton);
// }
//
// bool ScriptBindings::Input_MouseButtonDownICall(MouseButton mouseButton)
// {
//     return Input::IsMouseButtonDown(mouseButton);
// }
//
// bool ScriptBindings::Input_MouseButtonReleasedICall(MouseButton mouseButton)
// {
//     return Input::IsMouseButtonReleased(mouseButton);
// }
//
// void ScriptBindings::Input_GetMousePositionICall(glm::vec2& outMousePosition)
// {
//     glm::vec2 viewportPosition = SceneManager::GetCurrentScene()->GetViewportPosition();
//     outMousePosition = Input::GetMousePos() - viewportPosition;
// }
//
// bool ScriptBindings::Input_IsControllerConnectedICall(uint8_t controllerIndex)
// {
//     return Input::IsControllerConnected(controllerIndex);
// }
//
// Coral::String ScriptBindings::Input_GetControllerNameICall(uint8_t controllerIndex)
// {
//     return Coral::String::New(Input::GetControllerName(controllerIndex));
// }
// bool ScriptBindings::Input_IsControllerButtonPressedICall(uint8_t controllerIndex, ControllerButton controllerButton)
// {
//     return Input::IsControllerButtonPressed(controllerButton, controllerIndex);
// }
// float ScriptBindings::Input_GetControllerAxisICall(uint8_t controllerIndex, ControllerAxis controllerAxis)
// {
//     return Input::GetControllerAxis(controllerAxis, controllerIndex);
// }
// void* ScriptBindings::Input_GetConnectedControllersICall()
// {
//     std::vector<uint8_t> connectedControllers = Input::GetConnectedControllers();
//     Coral::Type* byteType = Coral::TypeCache::Get().GetTypeByName("System.Byte");
//     Coral::ManagedArray connectedControllersArray = Coral::ManagedArray::New(*byteType, static_cast<int32_t>(connectedControllers.size()));
//     for (size_t i = 0; i < connectedControllers.size(); i++)
//         connectedControllersArray.SetValue(static_cast<int32_t>(i), connectedControllers.at(i));
//
//     return connectedControllersArray.GetHandle();
// }
//
// #pragma endregion
// ---------------

// ---- Entity ----
// #pragma region Entity
// #define GET_ENTITY() \
//     World::Entity entity = World::SceneManager::GetCurrentScene()->FindEntityWithUUID(id)
//
// bool ScriptBindings::Entity_HasComponentICall(const Terran::Core::UUID& id, int32_t typeId)
// {
//     // TR_PROFILE_FUNCTION();
//     GET_ENTITY();
//     if (!entity)
//         return false;
//
//     if (s_HasComponentFuncs.find(typeId) != s_HasComponentFuncs.end())
//         return s_HasComponentFuncs.at(typeId)(entity);
//
//     Coral::Type* type = Coral::TypeCache::Get().GetTypeByID(typeId);
//     if (!type)
//         return false;
//
//     if (type->IsSubclassOf(*ScriptTypes::ScriptableType) && entity.HasComponent<ScriptComponent>()) {
//         Coral::ScopedString typeName = type->GetFullName();
//         return entity.GetComponent<ScriptComponent>().ModuleName == typeName;
//     }
//
//     return false;
// }
//
// void ScriptBindings::Entity_AddComponentICall(Terran::Core::UUID const& id, int32_t typeId)
// {
//     TR_PROFILE_FUNCTION();
//     GET_ENTITY();
//     if (!entity)
//         return;
//
//     if (s_AddComponentFuncs.find(typeId) != s_AddComponentFuncs.end()) {
//         s_AddComponentFuncs.at(typeId)(entity);
//         return;
//     }
//
//     Coral::Type* type = Coral::TypeCache::Get().GetTypeByID(typeId);
//     if (type->IsSubclassOf(*ScriptTypes::ScriptableType)) {
//         Coral::ScopedString typeName = type->GetFullName();
//         entity.AddComponent<ScriptComponent>((std::string)typeName);
//     }
// }
//
// void ScriptBindings::Entity_RemoveComponentICall(Terran::Core::UUID const& id, int32_t typeId)
// {
//     TR_PROFILE_FUNCTION();
//     GET_ENTITY();
//     if (!entity)
//         return;
//
//     if (s_RemoveComponentFuncs.find(typeId) != s_RemoveComponentFuncs.end())
//         s_RemoveComponentFuncs.at(typeId)(entity);
//
//     // TODO: handle removing script component
// }
//
// void* ScriptBindings::Entity_GetScriptableComponentICall(Terran::Core::UUID const& id)
// {
//     TR_PROFILE_FUNCTION();
//     GET_ENTITY();
//     if (!entity)
//         return nullptr;
//
//     Terran::Core::Shared<ScriptInstance> scriptInstance = ScriptEngine::GetScriptInstance(entity);
//     return (void*)scriptInstance->GetHandle();
// }
//
// bool ScriptBindings::Entity_FindEntityWithNameICall(Coral::String entityName, Terran::Core::UUID& id)
// {
//     Entity entity = SceneManager::GetCurrentScene()->FindEntityWithName(entityName);
//
//     if (!entity)
//         return false;
//
//     id = entity.GetID();
//     return true;
// }
//
// void ScriptBindings::Entity_DestroyEntityICall(Terran::Core::UUID const& id)
// {
//     TR_PROFILE_FUNCTION();
//     GET_ENTITY();
//
//     if (entity)
//         SceneManager::GetCurrentScene()->DestroyEntity(entity, true);
//     else
//         TR_CLIENT_ERROR("Can't destroy entity because it doesnt exist");
// }
//
// int ScriptBindings::Entity_GetChildrenCountICall(Terran::Core::UUID const& id)
// {
//     TR_PROFILE_FUNCTION();
//     GET_ENTITY();
//     if (!entity)
//         return 0;
//
//     return static_cast<int>(entity.GetChildCount());
// }
//
// Terran::Core::UUID ScriptBindings::Entity_GetChildICall(Terran::Core::UUID const& id, int index)
// {
//     TR_PROFILE_FUNCTION();
//     GET_ENTITY();
//     if (!entity)
//         return Terran::Core::UUID::Invalid();
//
//     return entity.GetChild(index).GetID();
// }
// Terran::Core::UUID ScriptBindings::Entity_GetParentICall(Terran::Core::UUID const& id)
// {
//     TR_PROFILE_FUNCTION();
//     GET_ENTITY();
//     if (!entity)
//         return Terran::Core::UUID::Invalid();
//
//     return entity.GetParentID();
// }
//
// #pragma endregion
// ----------------

// ---- Tag Component ----
// #pragma region Tag Component
// Coral::String ScriptBindings::Tag_GetNameICall(const Terran::Core::UUID& id)
// {
//     GET_ENTITY();
//     if (!entity)
//         return Coral::String::New("");
//
//     return Coral::String::New(entity.GetName());
// }
//
// void ScriptBindings::Tag_SetNameICall(Terran::Core::UUID const& id, Coral::String name)
// {
//     GET_ENTITY();
//     if (!entity)
//         return;
//
//     entity.GetComponent<TagComponent>().Name = name;
// }
// #pragma endregion
// // -----------------------
//
// #define SET_COMPONENT_PROPERTY(ComponentType, Property, Value)               \
//     Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id); \
//     if (!entity) {                                                           \
//         TR_CORE_ERROR(TR_LOG_SCRIPT, "Invalid entity id");                   \
//         return;                                                              \
//     }                                                                        \
//     entity.GetComponent<ComponentType>().Property = Value
//
// #define GET_COMPONENT_PROPERTY(ComponentType, Property, DefaultValue)        \
//     Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id); \
//     if (entity)                                                              \
//         return entity.GetComponent<ComponentType>().Property;                \
//                                                                              \
//     TR_CORE_ERROR(TR_LOG_SCRIPT, "Invalid entity id");                       \
//     return DefaultValue;
//
// #define GET_COMPONENT_PROPERTY_NORET(ComponentType, Property, Value)         \
//     Entity entity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id); \
//     if (entity) {                                                            \
//         Value = entity.GetComponent<ComponentType>().Property;               \
//         return;                                                              \
//     }                                                                        \
//                                                                              \
//     TR_CORE_ERROR(TR_LOG_SCRIPT, "Invalid entity id")
//
// #define SET_TRANSFORM_COMPONENT_PROPERTY(Property, Value)               \
//     GET_ENTITY();                                                       \
//     if (!entity) {                                                      \
//         TR_CORE_ERROR(TR_LOG_SCRIPT, "Invalid entity id");              \
//         return;                                                         \
//     }                                                                   \
//     TransformComponent& tc = entity.GetComponent<TransformComponent>(); \
//     tc.Property = Value;                                                \
//     tc.IsDirty = true
//
// // ---- Transform Component ----
// #pragma region Transform Component
// glm::vec3 ScriptBindings::Transform_GetPositionICall(const Terran::Core::UUID& id)
// {
//     GET_COMPONENT_PROPERTY(TransformComponent, Position, glm::vec3(0.0f, 0.0f, 0.0f));
// }
//
// void ScriptBindings::Transform_SetPositionICall(Terran::Core::UUID const& id, glm::vec3 const& position)
// {
//     SET_TRANSFORM_COMPONENT_PROPERTY(Position, position);
// }
//
// glm::vec3 ScriptBindings::Transform_GetRotationICall(Terran::Core::UUID const& id)
// {
//     GET_COMPONENT_PROPERTY(TransformComponent, Rotation, glm::vec3(0.0f, 0.0f, 0.0f));
// }
//
// void ScriptBindings::Transform_SetRotationICall(Terran::Core::UUID const& id, glm::vec3 const& rotation)
// {
//     SET_TRANSFORM_COMPONENT_PROPERTY(Rotation, rotation);
// }
//
// glm::vec3 ScriptBindings::Transform_GetScaleICall(Terran::Core::UUID const& id)
// {
//     GET_COMPONENT_PROPERTY(TransformComponent, Scale, glm::vec3(1.0f, 1.0f, 1.0f));
// }
//
// void ScriptBindings::Transform_SetScaleICall(Terran::Core::UUID const& id, glm::vec3 const& scale)
// {
//     SET_TRANSFORM_COMPONENT_PROPERTY(Scale, scale);
// }
//
// bool ScriptBindings::Transform_IsDirtyICall(Terran::Core::UUID const& id)
// {
//     GET_COMPONENT_PROPERTY(TransformComponent, IsDirty, false);
// }
//
// glm::vec3 ScriptBindings::Transform_GetForwardICall(Terran::Core::UUID const& id)
// {
//     GET_COMPONENT_PROPERTY(TransformComponent, Forward, glm::vec3(0.0f, 0.0f, 1.0f));
// }
//
// glm::vec3 ScriptBindings::Transform_GetUpICall(Terran::Core::UUID const& id)
// {
//     GET_COMPONENT_PROPERTY(TransformComponent, Up, glm::vec3(0.0f, 1.0f, 0.0f));
// }
// glm::vec3 ScriptBindings::Transform_GetRightICall(Terran::Core::UUID const& id)
// {
//     GET_COMPONENT_PROPERTY(TransformComponent, Right, glm::vec3(1.0f, 0.0f, 0.0f));
// }
// #pragma endregion
// -----------------------------

// ---- Window ----
// #pragma region Window
// float ScriptBindings::Window_GetWidthICall()
// {
//     Terran::Core::Shared<Scene> const& currentScene = SceneManager::GetCurrentScene();
//     return currentScene->GetViewportWidth();
// }
//
// float ScriptBindings::Window_GetHeightICall()
// {
//     Terran::Core::Shared<Scene> const& currentScene = SceneManager::GetCurrentScene();
//     return currentScene->GetViewportHeight();
// }
//
// bool ScriptBindings::Window_IsVSyncICall()
// {
//     return Application::Get()->GetWindow().IsVsync();
// }
//
// glm::vec2 ScriptBindings::Window_GetContentScaleICall()
// {
//     return Application::Get()->GetWindow().GetContentScale();
// }
// #pragma endregion
// ----------------

// ---- Scene ----
// #pragma region Scene
// Terran::Core::UUID ScriptBindings::Scene_GetMainCameraICall()
// {
//     Terran::Core::Shared<Scene> const& currentScene = SceneManager::GetCurrentScene();
//     Entity primaryCamera = currentScene->GetPrimaryCamera();
//     return primaryCamera.GetID();
// }
// #pragma endregion
// ---------------

// bool ScriptBindings::SceneManager_LoadSceneICall(Coral::String scenePath)
// {
//     AssetInfo const& sceneAssetInfo = AssetManager::GetAssetInfoByPath(std::string(std::string(scenePath) + ".terran"));
//     Terran::Core::Shared<Scene> loadedScene = AssetManager::GetAssetByAssetInfo<Scene>(sceneAssetInfo);
//     if (!loadedScene)
//         return false;
//
//     SceneManager::GetCurrentScene()->StopRuntime();
//
//     SceneManager::SetCurrentScene(Scene::CopyScene(loadedScene));
//     SceneManager::GetCurrentScene()->StartRuntime();
//
//     return true;
// }

// ------------------

}
