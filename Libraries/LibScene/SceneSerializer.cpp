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
#include <yaml-cpp/node/detail/iterator_fwd.h>
#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <fstream>

using namespace Terran::Core;
namespace Terran::World {

static char const* const SerializerVersion = "tr-0.1";

static void serialize_entity(YAML::Emitter& out, Entity entity)
{
    TR_ASSERT(entity.has_component<TagComponent>(), "Can't serialize an entity that doesn't have a tag component");

    out << YAML::BeginMap;
    out << YAML::Key << "Entity" << YAML::Value << entity.id();
    auto const& tag_component = entity.get_component<TagComponent>();
    out << YAML::Key << "TagComponent";
    out << YAML::BeginMap;
    out << YAML::Key << "Tag" << YAML::Value << tag_component.Name;
    out << YAML::EndMap;

    if (entity.has_component<TransformComponent>()) {
        auto const& transform_component = entity.transform();
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap;
        out << YAML::Key << "Position" << YAML::Value << Vec3ToYaml(transform_component.Position);
        out << YAML::Key << "Scale" << YAML::Value << Vec3ToYaml(transform_component.Scale);
        out << YAML::Key << "Rotation" << YAML::Value << Vec3ToYaml(transform_component.Rotation);
        out << YAML::EndMap;
    }

    if (entity.has_component<RelationshipComponent>()) {
        auto const& relationship_component = entity.get_component<RelationshipComponent>();

        out << YAML::Key << "RelationshipComponent";
        out << YAML::BeginMap;

        out << YAML::Key << "Children" << YAML::Value << YAML::BeginSeq;
        for (auto child : relationship_component.Children)
            out << child;
        out << YAML::EndSeq;

        out << YAML::Key << "Parent";
        if (relationship_component.Parent)
            out << YAML::Value << relationship_component.Parent;
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

    auto const tagComponentView = scene->entities_with<TagComponent>();
    for (auto e : tagComponentView) {
        Entity entity(e, scene.get());
        serialize_entity(out, entity);
    }

    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream ofs(assetMetadata.Path);
    ofs << out.c_str();
    return true;
}

static YAML::const_iterator find_entity(YAML::Node const& scene, Terran::Core::UUID const& entityID)
{
    for (YAML::const_iterator it = scene.begin(); it != scene.end(); ++it) {
        auto const& entity = *it;
        TR_CORE_TRACE(TR_LOG_CORE, entity);
        Terran::Core::UUID id = entity["Entity"].as<Terran::Core::UUID>();
        if (id == entityID)
            return it;
    }

    return scene.end();
}

static Entity deserialize_entity(YAML::Node const& data, YAML::Node const& scene, Core::Shared<Scene> deserializedScene)
{
    try {
        Core::UUID id = data["Entity"].as<Core::UUID>();
        if (!id) {
            TR_ASSERT(false, "Invalid id");
            return {};
        }

        if (Entity entity = deserializedScene->find_entity(id); entity)
            return entity;

        auto tagComponent = data["TagComponent"];
        if (!tagComponent) {
            TR_ASSERT(false, "Invalid tag component");
            return {};
        }

        std::string name = tagComponent["Tag"].as<std::string>();
        Entity deserializedEntity = deserializedScene->create_entity(name, id);

        if (auto transform_component_node = data["TransformComponent"]; transform_component_node) {
            auto& transform_component = deserializedEntity.transform();
            transform_component.Position = transform_component_node["Position"].as<glm::vec3>(glm::vec3(0.0f, 0.0f, 0.0f));
            transform_component.Rotation = transform_component_node["Rotation"].as<glm::vec3>(glm::vec3(0.0f, 0.0f, 0.0f));
            transform_component.Scale = transform_component_node["Scale"].as<glm::vec3>(glm::vec3(1.0f, 1.0f, 1.0f));
        }

        if (auto relationship_component_node = data["RelationshipComponent"]; relationship_component_node) {
            deserializedEntity.add_component<RelationshipComponent>();
            for (auto childID : relationship_component_node["Children"]) {
                Core::UUID deserializedChildID = childID.as<Core::UUID>();
                Entity child = deserializedScene->find_entity(deserializedChildID);
                if (!child) {
                    YAML::const_iterator childNode = find_entity(scene, deserializedChildID);
                    child = deserialize_entity(*childNode, scene, deserializedScene);
                }

                if (child)
                    child.set_parent(deserializedEntity);
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
        return { Core::CreateShared<SceneSerializerError>(SceneSerializerError::Code::InvalidFormat, ex.what()) };
    } catch (YAML::BadFile const& ex) {
        TR_CORE_ERROR(TR_LOG_CORE, ex.what());
        return { Core::CreateShared<SceneSerializerError>(SceneSerializerError::Code::NotFound, ex.what()) };
    }

    Core::Shared<Scene> scene = Core::CreateShared<Scene>(assetMetadata.Handle);

    if (auto entities = data["Entities"]; entities) {
        for (auto entity : entities) {
            if (!deserialize_entity(entity, entities, scene))
                return { Core::CreateShared<SceneSerializerError>(SceneSerializerError::Code::InvalidFormat) };
        }
    }

    return scene;
}

}
