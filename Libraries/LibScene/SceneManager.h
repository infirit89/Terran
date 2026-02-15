#pragma once

#include "LibCore/Base.h"
#include "LibCore/UUID.h"
#include "Scene.h"

#include <unordered_map>

namespace Terran::World {

class SceneManager final {
public:
    static Terran::Core::Shared<Scene> create_empty_scene();
    static void remove_scene(Terran::Core::UUID const& id);

    static Terran::Core::Shared<Scene> scene(Terran::Core::UUID const& id);

    static Terran::Core::Shared<Scene> const& current_scene() { return s_CurrentScene; }
    static void set_current_scene(Terran::Core::Shared<Scene> newScene);

    static std::unordered_map<Terran::Core::UUID, Terran::Core::Shared<Scene>>& active_scenes() { return s_ActiveScenes; }

private:
    static std::unordered_map<Terran::Core::UUID, Terran::Core::Shared<Scene>> s_ActiveScenes;
    static Terran::Core::Shared<Scene> s_CurrentScene;
};

}
