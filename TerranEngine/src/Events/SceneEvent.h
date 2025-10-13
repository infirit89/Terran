#pragma once

#include "LibCore/Event.h"
#include "LibCore/Base.h"

#include "Scene/Scene.h"

namespace TerranEngine {

class SceneTransitionEvent final : public Terran::Core::Event {
public:
    SceneTransitionEvent(Terran::Core::Weak<Scene> const& oldScene, Terran::Core::Shared<Scene> const& newScene)
        : m_OldScene(oldScene)
        , m_NewScene(newScene)
    {
    }

    EVENT_CLASS_TYPE(SceneTransitionEvent)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)

    Terran::Core::Weak<Scene> GetOldScene() { return m_OldScene; }
    Terran::Core::Shared<Scene> GetNewScene() { return m_NewScene; }

private:
    Terran::Core::Weak<Scene> m_OldScene;
    Terran::Core::Shared<Scene> m_NewScene;
};

}
