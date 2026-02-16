#include "SceneManager.h"
#include "Scene.h"
#include "SceneEvent.h"
#include "Components.h"
#include "Entity.h"

#include <LibCore/Base.h>
#include <LibCore/UUID.h>

namespace Terran::World {

Core::Shared<Scene> SceneManager::create_empty_scene()
{
    // TODO: create memory asset???
    Core::Shared<Scene> scene = Core::CreateShared<Scene>(event_dispatcher);
    m_active_scenes[scene->handle()] = scene;
    return scene;
}

void SceneManager::remove_scene(Core::UUID const& id)
{
    if (m_active_scenes.contains(id))
        m_active_scenes.erase(id);

    if (m_current_scene->handle() == id)
        m_current_scene = nullptr;
}

Core::Shared<Scene> SceneManager::scene(Core::UUID const& id)
{
    if (m_active_scenes.contains(id))
        return m_active_scenes.at(id);

    return nullptr;
}
Core::Shared<Scene> SceneManager::copy_scene(Core::Shared<Scene> const& source_scene)
{    
    Core::Shared<Scene> scene = Core::CreateShared<Scene>(event_dispatcher);

    auto tag_view = source_scene->entities_with<TagComponent>();

    for (auto e : tag_view) {
        Entity source_entity(e, source_scene.get());
        Entity destination_entity = scene->find_entity(source_entity.id());

        Scene::copy_component<TransformComponent>(
            source_entity,
            destination_entity,
            source_scene,
            scene);
        Scene::copy_component<RelationshipComponent>(
            source_entity,
            destination_entity,
            source_scene,
            scene);
    }

    scene->sort_entities();

    return scene;
}

void SceneManager::set_current_scene(Core::Shared<Scene> newScene)
{
    Core::UUID id({ 0 });
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
