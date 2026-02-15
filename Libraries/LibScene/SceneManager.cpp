#include "SceneManager.h"
#include "SceneEvent.h"
#include "Scene.h"

#include <LibCore/UUID.h>
#include <LibCore/Base.h>

namespace Terran::World {

Core::Shared<Scene> SceneManager::create_empty_scene()
{
    // TODO: create memory asset???
    Terran::Core::Shared<Scene> scene = Terran::Core::CreateShared<Scene>();
    m_active_scenes[scene->handle()] = scene;
    return scene;
}

void SceneManager::remove_scene(const Terran::Core::UUID& id)
{
    if (m_active_scenes.contains(id))
        m_active_scenes.erase(id);

    if (m_current_scene->handle() == id)
        m_current_scene = nullptr;
}

Terran::Core::Shared<Scene> SceneManager::scene(const Terran::Core::UUID& id)
{
    if (m_active_scenes.contains(id))
        return m_active_scenes.at(id);

    return nullptr;
}

void SceneManager::set_current_scene(Terran::Core::Shared<Scene> newScene)
{
    Terran::Core::UUID id({ 0 });
    if (m_current_scene)
        id = m_current_scene->handle();

    SceneTransitionEvent sceneTransitionEvent(m_current_scene, newScene);
    event_dispatcher.trigger(sceneTransitionEvent);

    m_current_scene = newScene;
    m_active_scenes[newScene->handle()] = newScene;

    if (!id)
        return;

    if (m_active_scenes.contains(id)) {
        if (m_active_scenes[id].use_count() > 1)
            return;

        m_active_scenes.erase(id);
    }
}

}
