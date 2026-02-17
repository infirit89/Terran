#include "SceneManager.h"
#include "Components.h"
#include "Entity.h"
#include "Scene.h"
#include "SceneEvent.h"

#include <LibCore/Base.h>
#include <LibCore/UUID.h>
#include <LibCore/RefPtr.h>

namespace Terran::World {

Core::RefPtr<Scene> SceneManager::create_empty_scene()
{
    return m_asset_system->asset_manager()->create_memory_asset<Scene>(event_dispatcher);
}

Core::RefPtr<Scene> SceneManager::copy_scene(Core::RefPtr<Scene> const& source_scene)
{
    Core::RefPtr<Scene> scene = m_asset_system->asset_manager()->create_memory_asset<Scene>(event_dispatcher);

    auto tag_view = source_scene->entities_with<TagComponent>();

    for (auto e : tag_view) {
        Entity source_entity(e, source_scene.data());
        Entity destination_entity = scene->find_entity(source_entity.id());

        Scene::copy_component<TransformComponent>(
            source_entity,
            destination_entity,
            source_scene->m_registry,
            scene->m_registry);
        Scene::copy_component<RelationshipComponent>(
            source_entity,
            destination_entity,
            source_scene->m_registry,
            scene->m_registry);
    }

    scene->sort_entities();

    return scene;
}

void SceneManager::set_current_scene(Core::RefPtr<Scene> new_scene)
{
    if (!m_current_scene) {
        return;
    }

    SceneTransitionEvent scene_transition_event(m_current_scene, new_scene);
    event_dispatcher.trigger(scene_transition_event);

    m_current_scene = new_scene;
}

}
