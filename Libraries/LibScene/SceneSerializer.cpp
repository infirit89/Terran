#include "SceneSerializer.h"
#include "Components.h"
#include "Entity.h"
#include "SceneSerializerError.h"

#include <LibAsset/AssetImporter.h>
#include <LibCore/Base.h>
#include <LibCore/Log.h>
#include <LibCore/Result.h>
#include <LibCore/SerializerExtensions.h>
#include <LibCore/UUID.h>

#include <yaml-cpp/anchor.h>
#include <yaml-cpp/emittermanip.h>
#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <filesystem>
#include <fstream>

using namespace Terran::Core;
namespace Terran::World {

static char const* SerializerVersion = "tr-0.1";

static void SerializeEntity(YAML::Emitter& out, Entity entity)
{
    // TODO: convert to a verify assert
    TR_ASSERT(entity.HasComponent<TagComponent>(), "Can't serialize an entity that doesn't have a tag component");

    out << YAML::BeginMap;
    out << YAML::Key << "Entity" << YAML::Value << entity.GetID();
    auto& tagComponent = entity.GetComponent<TagComponent>();
    out << YAML::Key << "TagComponent";
    out << YAML::BeginMap;
    out << YAML::Key << "Tag" << YAML::Value << tagComponent.Name;
    out << YAML::EndMap;

    if (entity.HasComponent<TransformComponent>()) {
        auto& transformComponent = entity.GetTransform();
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap;
        out << YAML::Key << "Position" << YAML::Value << transformComponent.Position;
        out << YAML::Key << "Scale" << YAML::Value << transformComponent.Scale;
        out << YAML::Key << "Rotation" << YAML::Value << transformComponent.Rotation;
        out << YAML::EndMap;
    }

    if (entity.HasComponent<RelationshipComponent>()) {
        auto& relationshipComponent = entity.GetComponent<RelationshipComponent>();

        out << YAML::Key << "RelationshipComponent";
        out << YAML::BeginMap;

        out << YAML::Key << "Children" << YAML::Value << YAML::BeginSeq;
        for (auto child : relationshipComponent.Children)
            out << child;
        out << YAML::EndSeq;

        out << YAML::Key << "Parent";
        if (relationshipComponent.Parent)
            out << YAML::Value << relationshipComponent.Parent;
        else
            out << YAML::Null;

        out << YAML::EndMap;
    }
    out << YAML::EndMap;
}

bool SceneSerializer::save(Asset::AssetMetadata const& assetMetadata, Core::Shared<Asset::Asset> const& asset)
{
    Core::Shared<Scene> scene = Core::DynamicCast<Scene>(asset);
    if (!scene)
        return false;

    YAML::Emitter out;

    out << YAML::BeginMap;

    out << YAML::Key << "SerializerVersion" << YAML::Value << SerializerVersion;
    out << YAML::Key << "Entities";
    out << YAML::BeginSeq;

    auto const tagComponentView = scene->GetEntitiesWith<TagComponent>();
    for (auto e : tagComponentView) {
        Entity entity(e, scene.get());
        SerializeEntity(out, entity);
    }

    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream ofs(assetMetadata.Path);
    ofs << out.c_str();
    return true;
}

static YAML::Node FindEntity(YAML::Node const& scene, Terran::Core::UUID const& entityID)
{
    for (auto entity : scene) {
        TR_CORE_TRACE(TR_LOG_CORE, entity);
        Terran::Core::UUID id = entity["Entity"].as<Terran::Core::UUID>();
        if (id == entityID)
            return entity;
    }

    return {};
}

static Entity DeserializeEntity(YAML::Node data, YAML::Node scene, Core::Shared<Scene> deserializedScene)
{
    try {
        Core::UUID id = data["Entity"].as<Core::UUID>();
        if (!id) {
            TR_ASSERT(false, "Invalid id");
            return {};
        }

        Entity entity = deserializedScene->FindEntityWithUUID(id);
        if (entity)
            return entity;

        auto tagComponent = data["TagComponent"];
        if (!tagComponent) {
            TR_ASSERT(false, "Invalid tag component");
            return {};
        }

        std::string name = tagComponent["Tag"].as<std::string>();
        Entity deserializedEntity = deserializedScene->CreateEntityWithUUID(name, id);

        auto transformComponent = data["TransformComponent"];
        if (transformComponent) {
            auto& tc = deserializedEntity.GetTransform();
            tc.Position = transformComponent["Position"].as<glm::vec3>(glm::vec3(0.0f, 0.0f, 0.0f));
            tc.Rotation = transformComponent["Rotation"].as<glm::vec3>(glm::vec3(0.0f, 0.0f, 0.0f));
            tc.Scale = transformComponent["Scale"].as<glm::vec3>(glm::vec3(1.0f, 1.0f, 1.0f));
        }

        auto relationshipComponent = data["RelationshipComponent"];
        if (relationshipComponent) {
            auto& rc = deserializedEntity.AddComponent<RelationshipComponent>();
            for (auto childID : relationshipComponent["Children"]) {
                Core::UUID deserializedChildID = childID.as<Core::UUID>();
                Entity child = deserializedScene->FindEntityWithUUID(deserializedChildID);
                if (!child) {
                    YAML::Node childNode = FindEntity(scene, deserializedChildID);
                    child = DeserializeEntity(childNode, scene, deserializedScene);
                }

                if (child)
                    child.SetParent(deserializedEntity, true);
            }
        }

        return deserializedEntity;
    } catch (YAML::InvalidNode const& ex) {
        TR_CORE_ERROR(TR_LOG_ASSET, ex.what());
        return Entity();
    }
}

Asset::AssetLoadResult SceneSerializer::load(Asset::AssetMetadata const& assetMetadata)
{
    YAML::Node data;
    try {
        data = YAML::LoadFile(assetMetadata.Path);
    } catch (YAML::ParserException const& ex) {
        return { Core::CreateShared<SceneSerializerError>(SceneSerializerError::InvalidFormat, ex.what()) };
    } catch (YAML::BadFile const& ex) {
        TR_CORE_ERROR(TR_LOG_CORE, ex.what());
        return { Core::CreateShared<SceneSerializerError>(SceneSerializerError::NotFound, ex.what()) };
    }

    Core::Shared<Scene> scene = Core::CreateShared<Scene>(assetMetadata.Handle);
    auto entities = data["Entities"];
    if (entities) {
        for (auto entity : entities) {
            if (!DeserializeEntity(entity, entities, scene))
                return { Core::CreateShared<SceneSerializerError>(SceneSerializerError::InvalidFormat) };
        }
    }

    return scene;
}

}
