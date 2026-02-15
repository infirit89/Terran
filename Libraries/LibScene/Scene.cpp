#include "Scene.h"

#include "Components.h"
#include "Entity.h"
#include "SceneManager.h"

#include <LibCore/Math.h>

// #include "Utils/Debug/OptickProfiler.h"
// #include "Utils/Debug/Profiler.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Terran::World {

struct SceneComponent final {
    Terran::Core::UUID SceneID;
};

namespace {

template<typename Component>
void CopyComponent(entt::entity srcHandle, entt::entity dstHandle, entt::registry& srcRegistry, entt::registry& dstRegistry)
{
    if (!srcRegistry.all_of<Component>(srcHandle))
        return;

    dstRegistry.emplace_or_replace<Component>(dstHandle, srcRegistry.get<Component>(srcHandle));
}

template<typename Component>
void CopyComponent(entt::entity srcHandle, entt::entity dstHandle, entt::registry& srcRegistry)
{
    CopyComponent<Component>(srcHandle, dstHandle, srcRegistry, srcRegistry);
}

static glm::mat4 CalculateTransformMatrix(TransformComponent const& transform)
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
    auto const sceneEntity = m_Registry.create();
    m_Registry.emplace<SceneComponent>(sceneEntity, m_handle);
}

Scene::~Scene()
{
    m_Registry.clear();
}

Entity Scene::CreateEntity(std::string const& name)
{
    return CreateEntityWithUUID(name, Terran::Core::UUID());
}

Entity Scene::CreateEntityWithUUID(std::string const& name, Terran::Core::UUID const& uuid)
{
    entt::entity e = m_Registry.create();

    Entity entity(e, this);
    entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name, uuid);
    entity.AddComponent<TransformComponent>();

    m_EntityMap[uuid] = e;

    SortEntities();
    return entity;
}

Entity Scene::CreateEmptyEntity()
{
    entt::entity e = m_Registry.create();
    Entity entity(e, this);
    return entity;
}

void Scene::DestroyEntity(Entity entity, bool first)
{
    // ScriptEngine::DestroyScriptInstance(entity);
    //
    // if (entity.HasComponent<Rigidbody2DComponent>())
    //     Physics2D::DestroyPhysicsBody(entity);

    if (entity.HasComponent<RelationshipComponent>()) {
        if (first) {
            if (entity.HasParent())
                entity.GetParent().RemoveChild(entity, false);
        }

        for (auto eID : entity.GetChildren())
            DestroyEntity(FindEntityWithUUID(eID), false);
    }

    auto entityIt = m_EntityMap.find(entity.GetID());
    if (entityIt != m_EntityMap.end())
        m_EntityMap.erase(entityIt);

    m_Registry.destroy(entity);

    SortEntities();
}

void Scene::StartRuntime()
{
    if (m_IsPlaying)
        return;

    m_IsPlaying = true;

    // Physics2D::CreatePhysicsWorld(Project::GetPhysicsSettings());
    // Physics2D::CratePhysicsBodies(this);
    //
    // auto scriptableComponentView = m_Registry.view<ScriptComponent>();
    //
    // for (auto e : scriptableComponentView) {
    //     Entity entity(e, this);
    //     ScriptEngine::OnStart(entity);
    // }
}

void Scene::StopRuntime()
{
    if (!m_IsPlaying)
        return;

    m_IsPlaying = false;
    // Physics2D::CleanUpPhysicsWorld();
}

void Scene::Update(Terran::Core::Time time)
{
    UpdateTransformHierarchy();
}


Entity Scene::FindEntityWithUUID(Terran::Core::UUID uuid)
{
    if (m_EntityMap.contains(uuid))
        return Entity(m_EntityMap.at(uuid), this);

    return {};
}

Entity Scene::FindEntityWithName(std::string const& name)
{
    auto const tagView = m_Registry.view<TagComponent>();

    for (auto e : tagView) {
        Entity entity(e, this);
        if (entity.GetName() == name)
            return entity;
    }

    return {};
}

Entity Scene::DuplicateEntity(Entity srcEntity, Entity parent)
{
    Entity dstEntity = CreateEntity(srcEntity.GetName() + " Copy");

    CopyComponent<TransformComponent>(srcEntity, dstEntity, m_Registry);

    if (srcEntity.HasComponent<RelationshipComponent>()) {
        for (int i = 0; i < srcEntity.GetChildCount(); i++) {
            Entity childEntity = srcEntity.GetChild(i);
            DuplicateEntity(childEntity, dstEntity);
        }

        if (!parent)
            parent = srcEntity.GetParent();

        if (parent)
            dstEntity.SetParent(parent);
    }

    return dstEntity;
}

Entity Scene::DuplicateEntity(Entity srcEntity)
{
    return DuplicateEntity(srcEntity, {});
}

Terran::Core::Shared<Scene> Scene::CopyScene(Terran::Core::Shared<Scene> const& srcScene)
{
    Terran::Core::Shared<Scene> scene = SceneManager::CreateEmptyScene();

    auto tagView = srcScene->GetEntitiesWith<TagComponent>();

    for (auto e : tagView) {
        Entity srcEntity(e, srcScene.get());
        Entity dstEntity = scene->CreateEntityWithUUID(srcEntity.GetName(), srcEntity.GetID());
    }

    for (auto e : tagView) {
        Entity srcEntity(e, srcScene.get());
        Entity dstEntity = scene->FindEntityWithUUID(srcEntity.GetID());

        CopyComponent<TransformComponent>(srcEntity, dstEntity, srcScene->m_Registry, scene->m_Registry);
        CopyComponent<RelationshipComponent>(srcEntity, dstEntity, srcScene->m_Registry, scene->m_Registry);
    }

    scene->SortEntities();

    return scene;
}

void Scene::UpdateTransformHierarchy()
{
    auto transformView = GetEntitiesWith<TransformComponent>();

    for (auto e : transformView) {
        Entity entity(e, this);
        auto& transform = entity.GetTransform();

        if (!entity.HasParent())
            UpdateEntityTransform(entity);
    }
}

void Scene::UpdateEntityTransform(Entity entity)
{
    TransformComponent& tc = entity.GetComponent<TransformComponent>();

    if (tc.IsDirty) {
        if (Entity parent = entity.GetParent()) {
            glm::mat4 parentTransform = parent.GetTransform().WorldSpaceTransformMatrix;
            tc.WorldSpaceTransformMatrix = parentTransform * CalculateTransformMatrix(tc);
            tc.LocalSpaceTransformMatrix = glm::inverse(parentTransform) * tc.WorldSpaceTransformMatrix;
        } else {
            tc.WorldSpaceTransformMatrix = CalculateTransformMatrix(tc);
            tc.LocalSpaceTransformMatrix = tc.WorldSpaceTransformMatrix;
        }

        glm::quat rotation = tc.Rotation;

        tc.Forward = glm::normalize(glm::rotate(rotation, glm::vec3(0.0f, 0.0f, 1.0f)));
        tc.Up = glm::normalize(glm::rotate(rotation, glm::vec3(0.0f, 1.0f, 0.0f)));
        tc.Right = glm::normalize(glm::rotate(rotation, glm::vec3(1.0f, 0.0f, 0.0f)));
    }

    for (size_t i = 0; i < entity.GetChildCount(); i++) {
        Entity currEntity = entity.GetChild(static_cast<uint32_t>(i));

        if (tc.IsDirty)
            currEntity.GetTransform().IsDirty = true;

        UpdateEntityTransform(currEntity);
    }

    tc.IsDirty = false;
}

void Scene::ConvertToLocalSpace(Entity entity)
{
    auto& tc = entity.GetComponent<TransformComponent>();

    if (!entity.HasParent())
        return;

    if (tc.IsDirty)
        UpdateEntityTransform(entity);

    Entity parent = entity.GetParent();
    auto& parentTransform = parent.GetTransform();

    // NOTE: have to calculate it because at this point the local space
    //		and world space transform matrices are equal
    glm::mat4 parentWorldMatrix = parentTransform.WorldSpaceTransformMatrix;
    glm::mat4 localMat = glm::inverse(parentWorldMatrix) * tc.WorldSpaceTransformMatrix;

    Core::Math::decompose_transform_matrix(localMat, tc.Position, tc.Rotation, tc.Scale);

    tc.IsDirty = true;
}

void Scene::ConvertToWorldSpace(Entity entity)
{
    auto& tc = entity.GetComponent<TransformComponent>();

    if (!entity.HasParent())
        return;

    if (tc.IsDirty)
        UpdateEntityTransform(entity);

    Core::Math::decompose_transform_matrix(tc.WorldSpaceTransformMatrix, tc.Position, tc.Rotation, tc.Scale);

    tc.IsDirty = true;
}

void Scene::SortEntities()
{
    m_Registry.sort<TagComponent>([](entt::entity const& lEntity, entt::entity const& rEntity) { return lEntity < rEntity; });
}

}
