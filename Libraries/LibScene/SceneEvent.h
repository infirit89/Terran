#pragma once

#include "Scene.h"

#include <LibCore/Base.h>
#include <LibCore/Event.h>

namespace Terran::World {

class SceneTransitionEvent {
public:
    SceneTransitionEvent(Core::Weak<Scene> const& oldScene, Core::Shared<Scene> const& newScene)
        : m_old_scene(oldScene)
        , m_new_scene(newScene)
    {
    }

    Core::Weak<Scene> old_scene() const
    {
        return m_old_scene;
    }

    Core::Shared<Scene> new_scene() const
    {
        return m_new_scene;
    }

private:
    Core::Weak<Scene> m_old_scene;
    Core::Shared<Scene> m_new_scene;
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
