#include "Scene.h"

#include "Components.h"
#include "Entity.h"

#include <LibCore/Base.h>
#include <LibCore/Math.h>
#include <LibCore/Time.h>
#include <LibCore/UUID.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <string_view>

namespace Terran::World {

struct SceneComponent final {
    Terran::Core::UUID SceneID;
};

namespace {

template<typename Component>
void copy_component(entt::entity srcHandle, entt::entity dstHandle, entt::registry& srcRegistry, entt::registry& dstRegistry)
{
    if (!srcRegistry.all_of<Component>(srcHandle))
        return;

    dstRegistry.emplace_or_replace<Component>(dstHandle, srcRegistry.get<Component>(srcHandle));
}

template<typename Component>
void copy_component(entt::entity srcHandle, entt::entity dstHandle, entt::registry& srcRegistry)
{
    copy_component<Component>(srcHandle, dstHandle, srcRegistry, srcRegistry);
}

glm::mat4 calculate_transform_matrix(TransformComponent const& transform)
{
    return glm::translate(glm::mat4(1.0f), transform.Position) * glm::toMat4(glm::quat(transform.Rotation)) * glm::scale(glm::mat4(1.0f), transform.Scale);
}

}

Scene::Scene()
    : Scene(Terran::Core::UUID())
{
}

Scene::Scene(Terran::Core::UUID const& handle)
    : Asset(handle)
{
    auto const sceneEntity = m_registry.create();
    m_registry.emplace<SceneComponent>(sceneEntity, m_handle);
}

Scene::~Scene()
{
    m_registry.clear();
}

Entity Scene::create_entity(std::string const& name)
{
    return create_entity(name, Terran::Core::UUID());
}

Entity Scene::create_entity(std::string const& name, Terran::Core::UUID const& uuid)
{
    entt::entity e = m_registry.create();

    Entity entity(e, this);
    entity.add_component<TagComponent>(name.empty() ? "Entity" : name, uuid);
    entity.add_component<TransformComponent>();

    m_entity_map[uuid] = e;

    sort_entities();
    return entity;
}

Entity Scene::create_entity()
{
    entt::entity e = m_registry.create();
    Entity entity(e, this);
    return entity;
}

void Scene::destrory_entity(Entity entity, bool first)
{
    if (entity.has_component<RelationshipComponent>()) {
        if (first && entity.has_parent()) {
            entity.parent().remove_child(entity);
        }

        for (auto const& child_id : entity.children())
            destrory_entity(find_entity(child_id), false);
    }

    if (auto entity_iterator = m_entity_map.find(entity.id()); entity_iterator != m_entity_map.end())
        m_entity_map.erase(entity_iterator);

    m_registry.destroy((entt::entity)entity);

    sort_entities();
}

void Scene::start_runtime()
{
    if (m_is_playing)
        return;

    m_is_playing = true;

    // TODO: fire an on start playing/start simulation event
}

void Scene::stop_runtime()
{
    if (!m_is_playing)
        return;

    m_is_playing = false;
    // TODO: fire an on stop playing/stop simulation event
}

void Scene::update(Terran::Core::Time)
{
    update_transform_hierarchy();
}

Entity Scene::find_entity(Terran::Core::UUID uuid)
{
    if (m_entity_map.contains(uuid))
        return Entity(m_entity_map.at(uuid), this);

    return {};
}

Entity Scene::find_entity(std::string_view name)
{
    auto const tag_view = m_registry.view<TagComponent>();

    for (auto e : tag_view) {
        Entity entity(e, this);
        if (entity.name() == name)
            return entity;
    }

    return {};
}

Entity Scene::duplicate_entity(Entity source_entity, Entity parent)
{
    Entity destination_entity = create_entity(source_entity.name() + " Copy");

    copy_component<TransformComponent>(
        (entt::entity)source_entity,
        (entt::entity)destination_entity,
        m_registry);

    if (source_entity.has_component<RelationshipComponent>()) {
        for (int i = 0; i < source_entity.children_count(); i++) {
            Entity childEntity = source_entity.child_at(i);
            duplicate_entity(childEntity, destination_entity);
        }

        if (!parent)
            parent = source_entity.parent();

        if (parent)
            destination_entity.set_parent(parent);
    }

    return destination_entity;
}

Entity Scene::duplicate_entity(Entity srcEntity)
{
    return duplicate_entity(srcEntity, {});
}

Terran::Core::Shared<Scene> Scene::copy_scene(Terran::Core::Shared<Scene> const& source_scene)
{
    Core::Shared<Scene> scene = Core::CreateShared<Scene>();

    auto tag_view = source_scene->entities_with<TagComponent>();

    for (auto e : tag_view) {
        Entity source_entity(e, source_scene.get());
        Entity destination_entity = scene->find_entity(source_entity.id());

        copy_component<TransformComponent>(
            (entt::entity)source_entity,
            (entt::entity)destination_entity,
            source_scene->m_registry,
            scene->m_registry);
        copy_component<RelationshipComponent>(
            (entt::entity)source_entity,
            (entt::entity)destination_entity,
            source_scene->m_registry,
            scene->m_registry);
    }

    scene->sort_entities();

    return scene;
}

void Scene::update_transform_hierarchy()
{
    auto transform_view = entities_with<TransformComponent>();

    for (auto e : transform_view) {
        Entity entity(e, this);

        if (!entity.has_parent())
            update_entity_transform(entity);
    }
}

void Scene::update_entity_transform(Entity entity)
{
    TransformComponent& transform_component = entity.get_component<TransformComponent>();

    if (transform_component.IsDirty) {
        if (Entity parent = entity.parent()) {
            glm::mat4 parentTransform = parent.transform().WorldSpaceTransformMatrix;
            transform_component.WorldSpaceTransformMatrix = parentTransform * calculate_transform_matrix(transform_component);
            transform_component.LocalSpaceTransformMatrix = glm::inverse(parentTransform) * transform_component.WorldSpaceTransformMatrix;
        } else {
            transform_component.WorldSpaceTransformMatrix = calculate_transform_matrix(transform_component);
            transform_component.LocalSpaceTransformMatrix = transform_component.WorldSpaceTransformMatrix;
        }

        glm::quat rotation = transform_component.Rotation;

        transform_component.Forward = glm::normalize(glm::rotate(rotation, glm::vec3(0.0f, 0.0f, 1.0f)));
        transform_component.Up = glm::normalize(glm::rotate(rotation, glm::vec3(0.0f, 1.0f, 0.0f)));
        transform_component.Right = glm::normalize(glm::rotate(rotation, glm::vec3(1.0f, 0.0f, 0.0f)));
    }

    for (size_t i = 0; i < entity.children_count(); i++) {
        Entity currEntity = entity.child_at(static_cast<uint32_t>(i));

        if (transform_component.IsDirty)
            currEntity.transform().IsDirty = true;

        update_entity_transform(currEntity);
    }

    transform_component.IsDirty = false;
}

void Scene::convert_to_local_space(Entity entity)
{
    auto& transform_component = entity.get_component<TransformComponent>();

    if (!entity.has_parent())
        return;

    if (transform_component.IsDirty)
        update_entity_transform(entity);

    Entity parent = entity.parent();
    auto const& parent_transform_component = parent.transform();

    // NOTE: have to calculate it because at this point the local space
    //		and world space transform matrices are equal
    glm::mat4 parent_world_matrix = parent_transform_component.WorldSpaceTransformMatrix;
    glm::mat4 local_matrix = glm::inverse(parent_world_matrix) * transform_component.WorldSpaceTransformMatrix;

    Core::Math::decompose_transform_matrix(local_matrix, transform_component.Position, transform_component.Rotation, transform_component.Scale);

    transform_component.IsDirty = true;
}

void Scene::convert_to_world_space(Entity entity)
{
    auto& transform_component = entity.get_component<TransformComponent>();

    if (!entity.has_parent())
        return;

    if (transform_component.IsDirty)
        update_entity_transform(entity);

    Core::Math::decompose_transform_matrix(transform_component.WorldSpaceTransformMatrix, transform_component.Position, transform_component.Rotation, transform_component.Scale);

    transform_component.IsDirty = true;
}

void Scene::sort_entities()
{
    m_registry.sort<TagComponent>([](entt::entity const& lEntity, entt::entity const& rEntity) { return lEntity < rEntity; });
}

}
