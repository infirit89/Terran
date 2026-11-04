#include "ScriptBindings.h"

#include <LibScene/Entity.h>
#include <LibScene/SceneManager.h>

// #include "Utils/Debug/OptickProfiler.h"

#include <glm/glm.hpp>

#include <Coral/Assembly.hpp>
#include <Coral/ManagedArray.hpp>
#include <Coral/Type.hpp>
#include <Coral/TypeCache.hpp>

namespace Terran::Script {

#define BIND_INTERNAL_FUNC(func) assembly.AddInternalCall("Terran.Internal", #func, reinterpret_cast<void*>(&func))


void ScriptBindings::Bind(Coral::ManagedAssembly& assembly)
{
//
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
    // s_HasComponentFuncs.clear();
    // s_AddComponentFuncs.clear();
    // s_RemoveComponentFuncs.clear();
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
