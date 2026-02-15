#pragma once

#include <LibCore/Base.h>
#include <LibCore/UUID.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <string>

namespace Terran::World {

struct TagComponent {
    TagComponent() = default;

    TagComponent(std::string const& name, Terran::Core::UUID const& id)
        : Name(name)
        , ID(id)
    {
    }

    explicit TagComponent(std::string const& name)
        : Name(name)
    {
    }

    std::string Name;
    Terran::Core::UUID ID;
};

struct TransformComponent {
    TransformComponent() = default;

    glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

    glm::vec3 Forward = { 0.0f, 0.0f, 1.0f };
    glm::vec3 Up = { 0.0f, 1.0f, 0.0f };
    glm::vec3 Right = { 1.0f, 0.0f, 0.0f };

    // dirty flag used for optimization
    bool IsDirty = true;

    // cached transform matrices
    glm::mat4 WorldSpaceTransformMatrix = glm::mat4(1.0f);
    glm::mat4 LocalSpaceTransformMatrix = glm::mat4(1.0f);
};

struct RelationshipComponent {
    RelationshipComponent() = default;

    Core::UUID Parent = Core::UUID::invalid();
    std::vector<Core::UUID> Children;
};

}
