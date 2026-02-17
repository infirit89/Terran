#pragma once

#include "Scene.h"

#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/WeakRef.h>
#include <LibCore/RefPtr.h>

namespace Terran::World {

class SceneTransitionEvent {
public:
    SceneTransitionEvent(Core::WeakPtr<Scene> const& oldScene, Core::RefPtr<Scene> const& newScene)
        : m_old_scene(oldScene)
        , m_new_scene(newScene)
    {
    }

    Core::WeakPtr<Scene> old_scene() const
    {
        return m_old_scene;
    }

    Core::RefPtr<Scene> new_scene() const
    {
        return m_new_scene;
    }

private:
    Core::WeakPtr<Scene> m_old_scene;
    Core::RefPtr<Scene> m_new_scene;
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
