#pragma once

#include "Scene.h"

#include <LibCore/Base.h>
#include <LibCore/Event.h>

namespace Terran::World {

class SceneTransitionEvent {
public:
    SceneTransitionEvent(Terran::Core::Weak<Scene> const& oldScene, Terran::Core::Shared<Scene> const& newScene)
        : m_old_scene(oldScene)
        , m_new_scene(newScene)
    {
    }

    Terran::Core::Weak<Scene> old_scene() { return m_old_scene; }
    Terran::Core::Shared<Scene> new_scene() { return m_new_scene; }

private:
    Terran::Core::Weak<Scene> m_old_scene;
    Terran::Core::Shared<Scene> m_new_scene;
};

class SceneStartSimulationEvent {
public:
    SceneStartSimulationEvent() = default;
};

class SceneStopSimulationEvent {
public:
    SceneStopSimulationEvent() = default;
};

}
