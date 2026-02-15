#pragma once

#include "Entity.h"
#include <concepts>
#include <yaml-cpp/yaml.h>
namespace Terran::World {

template<typename T>
concept ComponentSerializer = requires(
    T serializer, YAML::Emitter& out, YAML::Node& node, Entity entity) {
    { serializer.serialize(out, entity) } -> std::same_as<void>;
    { serializer.deserialize(node, entity) } -> std::same_as<void>;
};

}
