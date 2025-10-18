#include "Input.h"

#include <LibCore/Log.h>
#include <variant>

namespace Terran {
namespace Window {

static std::unordered_map<Key, InputState> s_KeyStates;
static std::unordered_map<MouseButton, InputState> s_MouseButtonStates;

void Input::Init()
{
    std::variant<class Types>
    s_KeyStates.reserve(sizeof(InputUtils::Keys));
    s_MouseButtonStates.reserve(sizeof(InputUtils::MouseButtons));

    for (auto& key : InputUtils::Keys)
        s_KeyStates[key] = {};

    for (auto& mouseButton : InputUtils::MouseButtons)
        s_MouseButtonStates[mouseButton] = {};

    TR_CORE_INFO(TR_LOG_CORE, "Initialized input system");
}

void Input::Update()
{
    for (auto& [key, keyState] : s_KeyStates) {
        keyState.PreviousState = keyState.CurrentState;
        keyState.CurrentState = GetKeyState(key);
    }

    for (auto& [mouseButton, mouseButtonState] : s_MouseButtonStates) {
        mouseButtonState.PreviousState = mouseButtonState.CurrentState;
        mouseButtonState.CurrentState = GetMouseButtonState(mouseButton);
    }
}

bool Input::IsKeyPressed(Key key) { return s_KeyStates.at(key).PreviousState == false && s_KeyStates[key].CurrentState == true; }
bool Input::IsKeyDown(Key key) { return s_KeyStates.at(key).CurrentState == true; }
bool Input::IsKeyReleased(Key key) { return s_KeyStates.at(key).PreviousState == true && s_KeyStates[key].CurrentState == false; }

bool Input::IsMouseButtonPressed(MouseButton button)
{
    return s_MouseButtonStates[button].PreviousState == false && s_MouseButtonStates[button].CurrentState == true;
}
bool Input::IsMouseButtonDown(MouseButton button) { return s_MouseButtonStates[button].CurrentState == true; }
bool Input::IsMouseButtonReleased(MouseButton button)
{
    return s_MouseButtonStates[button].PreviousState == true && s_MouseButtonStates[button].CurrentState == false;
}

std::unordered_map<MouseButton, InputState> Input::GetMouseButtonStates() { return s_MouseButtonStates; }
std::unordered_map<Key, InputState> Input::GetKeyStates() { return s_KeyStates; }

}

}
