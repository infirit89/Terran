#pragma once

#include "Scene.h"

#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/Layer.h>
#include <LibCore/UUID.h>

#include <unordered_map>

namespace Terran::World {

class SceneManager final : public Core::Layer {
    using active_scenes_container = std::unordered_map<Core::UUID, Core::Shared<Scene>>;

public:
    SceneManager(Core::EventDispatcher& dispatcher)
        : Core::Layer("Scene", dispatcher)
    {
    }

    Core::Shared<Scene> create_empty_scene();

    void remove_scene(Terran::Core::UUID const& id);

    Core::Shared<Scene> scene(Terran::Core::UUID const& id);

    Core::Shared<Scene> const& current_scene()
    {
        return m_current_scene;
    }

    void set_current_scene(Terran::Core::Shared<Scene> newScene);
    Core::Shared<Scene> copy_scene(Core::Shared<Scene> const& source_scene);

    active_scenes_container& active_scenes()
    {
        return m_active_scenes;
    }

private:
    active_scenes_container m_active_scenes;
    Core::Shared<Scene> m_current_scene;
};

}
