#include "SceneSerializer.h"
#include "Components.h"
#include "Entity.h"
#include "SceneManager.h"
#include "SceneSerializerError.h"
#include "SceneTypes.h"

#include <LibCore/Base.h>
#include <LibCore/Log.h>
#include <LibCore/RefPtr.h>
#include <LibCore/Result.h>
#include <LibCore/SerializerExtensions.h>
#include <LibCore/UUID.h>

#include <LibAsset/Asset.h>
#include <LibAsset/AssetImporter.h>

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

bool SceneSerializer::save(Asset::AssetMetadata const& assetMetadata, Core::RefPtr<Asset::Asset> const& asset)
{
    Core::RefPtr<Scene> scene = Core::dynamic_pointer_cast<Scene>(asset);
    if (!scene)
        return false;

    YAML::Emitter out;

    out << YAML::BeginMap;

    out << YAML::Key << "SerializerVersion" << YAML::Value << SerializerVersion;
    out << YAML::Key << "Entities";
    out << YAML::BeginSeq;

    auto const tagComponentView = scene->entities_with<TagComponent>();
    for (auto e : tagComponentView) {
        Entity entity(e, scene.data());
        serialize_entity(out, entity);
    }

    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream ofs(assetMetadata.Path);
    ofs << out.c_str();
    return true;
}

static YAML::const_iterator find_entity_node(YAML::Node const& scene, Terran::Core::UUID const& entityID)
{
    for (auto it = scene.begin(); it != scene.end(); ++it) {
        auto const& entity = *it;
        Terran::Core::UUID id = entity["Entity"].as<Terran::Core::UUID>();
        if (id == entityID)
            return it;
    }

    return scene.end();
}

static Entity deserialize_entity(YAML::Node const& data, YAML::Node const& scene, Core::RefPtr<Scene> deserializedScene)
{
    try {
        Core::UUID id = data["Entity"].as<Core::UUID>();
        if (!id) {
            TR_ERROR(TR_LOG_SCENE, "Invalid id");
            TR_ASSERT(false, "Invalid id");
            return {};
        }

        if (Entity entity = deserializedScene->find_entity(id); entity)
            return entity;

        auto tag_component_node = data["TagComponent"];
        if (!tag_component_node) {
            TR_ASSERT(false, "Invalid tag component");
            return {};
        }

        std::string name = tag_component_node["Tag"].as<std::string>();
        Entity deserialized_entity = deserializedScene->create_entity(name, id);

        if (auto transform_component_node = data["TransformComponent"]; transform_component_node) {
            auto& transform_component = deserialized_entity.transform();
            transform_component.Position = transform_component_node["Position"].as<glm::vec3>(glm::vec3(0.0f, 0.0f, 0.0f));
            transform_component.Rotation = transform_component_node["Rotation"].as<glm::vec3>(glm::vec3(0.0f, 0.0f, 0.0f));
            transform_component.Scale = transform_component_node["Scale"].as<glm::vec3>(glm::vec3(1.0f, 1.0f, 1.0f));
        }

        if (auto relationship_component_node = data["RelationshipComponent"]; relationship_component_node) {
            deserialized_entity.add_component<RelationshipComponent>();
            for (auto child_id_node : relationship_component_node["Children"]) {
                Core::UUID deserialized_child_id = child_id_node.as<Core::UUID>();
                Entity child = deserializedScene->find_entity(deserialized_child_id);

                if (child) {
                    child.set_parent(deserialized_entity);
                    continue;
                }

                YAML::const_iterator child_node = find_entity_node(scene, deserialized_child_id);
                if (child_node == scene.end()) {
                    TR_WARN(TR_LOG_SCENE, "Entity {} references child entity with Id: {} could not be found! Child is skipped!", id, deserialized_child_id);
                } else {
                    child = deserialize_entity(*child_node, scene, deserializedScene);
                }

                child.set_parent(deserialized_entity);
            }
        }

        return deserialized_entity;
    } catch (YAML::InvalidNode const& ex) {
        TR_ERROR(TR_LOG_CORE, ex.what());
        return Entity();
    }
}

Asset::AssetLoadResult SceneSerializer::load(Asset::AssetMetadata const& assetMetadata)
{
    YAML::Node data;
    try {
        data = YAML::LoadFile(assetMetadata.Path);
    } catch (YAML::ParserException const& ex) {
        TR_ERROR(TR_LOG_SCENE, ex.what());
        return { Core::CreateShared<SceneSerializerError>(SceneSerializerError::Code::InvalidFormat, ex.what()) };
    } catch (YAML::BadFile const& ex) {
        TR_ERROR(TR_LOG_SCENE, ex.what());
        return { Core::CreateShared<SceneSerializerError>(SceneSerializerError::Code::NotFound, ex.what()) };
    }

    Core::RefPtr<Scene> scene = m_scene_system.create_empty_scene();

    if (auto entities = data["Entities"]; entities) {
        for (auto entity : entities) {
            if (!deserialize_entity(entity, entities, scene))
                return { Core::CreateShared<SceneSerializerError>(SceneSerializerError::Code::InvalidFormat) };
        }
    }

    return scene;
}

}
