#pragma once

#include "ControllerIndices.h"
#include "CursorStates.h"
#include "KeyCodes.h"
#include "MouseButtons.h"

#include <glm/glm.hpp>

#include <vector>

namespace Terran {
namespace Window {

struct InputState final {
    bool CurrentState = false;
    bool PreviousState = false;
};

class Input final {
public:
    static void Init();

    static bool IsKeyPressed(Key key);
    static bool IsKeyDown(Key key);
    static bool IsKeyReleased(Key key);

    static bool GetKeyState(Key key);
    static bool GetMouseButtonState(MouseButton button);

    static bool IsMouseButtonPressed(MouseButton button);
    static bool IsMouseButtonDown(MouseButton button);
    static bool IsMouseButtonReleased(MouseButton button);

    static void SetCursorState(CursorMode cursorMode);

    static glm::vec2 GetMousePos();

    static bool IsControllerConnected(uint8_t controllerIndex);
    static char const* GetControllerName(uint8_t controllerIndex);

    static bool IsControllerButtonPressed(ControllerButton controllerButton, uint8_t controllerIndex);
    static float GetControllerAxis(ControllerAxis controllerAxis, uint8_t controllerIndex);

    static glm::vec2 GetControllerRightStick(uint8_t controllerIndex);
    static glm::vec2 GetControllerLeftStick(uint8_t controllerIndex);

    static std::vector<uint8_t> GetConnectedControllers();
    static float GetMouseX() { return GetMousePos().x; }
    static float GetMouseY() { return GetMousePos().y; }

    static void Update();

    static std::unordered_map<Key, InputState> GetKeyStates();
    static std::unordered_map<MouseButton, InputState> GetMouseButtonStates();
};

}

}
