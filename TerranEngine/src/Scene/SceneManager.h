#pragma once

#include "LibCore/Base.h"
#include "LibCore/UUID.h"
#include "Scene.h"

#include <unordered_map>

namespace TerranEngine {

class SceneManager final {
public:
    static Terran::Core::Shared<Scene> CreateEmptyScene();
    static void RemoveScene(Terran::Core::UUID const& id);

    static Terran::Core::Shared<Scene> GetScene(Terran::Core::UUID const& id);

    static Terran::Core::Shared<Scene> const& GetCurrentScene() { return s_CurrentScene; }
    static void SetCurrentScene(Terran::Core::Shared<Scene> newScene);

    static std::unordered_map<Terran::Core::UUID, Terran::Core::Shared<Scene>>& GetActiveScenes() { return s_ActiveScenes; }

private:
    static std::unordered_map<Terran::Core::UUID, Terran::Core::Shared<Scene>> s_ActiveScenes;
    static Terran::Core::Shared<Scene> s_CurrentScene;
};

}
