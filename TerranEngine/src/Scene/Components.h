#pragma once

#include "LibCore/Base.h"
#include "LibCore/UUID.h"

#include "Graphics/Font.h"
#include "Graphics/OrthographicCamera.h"

#include "Physics/PhysicsStates.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <string>
#include <cstdint>

namespace TerranEngine {

struct TagComponent {
    TagComponent() = default;

    TagComponent(std::string const& name, Terran::Core::UUID const& id)
        : Name(name)
        , ID(id)
    {
    }

    TagComponent(std::string const& name)
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

struct CameraComponent {
    CameraComponent() = default;

    OrthographicCamera Camera;
    bool Primary = true;

    glm::vec4 BackgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f };
};

struct SpriteRendererComponent {
    SpriteRendererComponent() = default;

    glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
    Terran::Core::UUID TextureHandle = Terran::Core::UUID::invalid();

    int ZIndex = 0;
};

struct CircleRendererComponent {
    CircleRendererComponent() = default;

    glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
    float Thickness = 1.0f;
};

// bullshit; fix
struct LineRendererComponent {
    LineRendererComponent() = default;

    glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
    float Thickness = 1.0f;

    glm::vec3 StartPoint = { 0.0f, 0.0f, 0.0f };
    glm::vec3 EndPoint = { 0.0f, 0.0f, 1.0f };
};

struct TextRendererComponent {
    TextRendererComponent() = default;
    ~TextRendererComponent() = default;

    Terran::Core::Shared<Font> FontAtlas;
    glm::vec4 TextColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    std::string Text = "";
    float LineSpacing = 1.0f;
    float LineWidth = 10.0f;
};

struct RelationshipComponent {
    RelationshipComponent()
        : Parent({ 0 })
    {
    }

    Terran::Core::UUID Parent;
    std::vector<Terran::Core::UUID> Children;
};

struct ScriptComponent {
    ScriptComponent() = default;

    ScriptComponent(std::string const& moduleName)
        : ModuleName(moduleName)
    {
    }

    // NOTE: think about having an array of scripts so that one entity
    // "can" have more than one script (because of the 1 component of a type per entity)

    std::string ModuleName;
    std::vector<int32_t> FieldHandles;

    Terran::Core::UUID ScriptSourceHandle = Terran::Core::UUID::invalid();
    bool ClassExists = true;
};

struct Rigidbody2DComponent {
    Rigidbody2DComponent() = default;

    PhysicsBodyType BodyType = PhysicsBodyType::Dynamic;
    PhysicsBodySleepState SleepState = PhysicsBodySleepState::Awake;

    bool FixedRotation = false;
    float GravityScale = 1.0f;
    bool Enabled = true;
    int LayerIndex = 0;

    Terran::Core::UUID PhysicsMaterialHandle = Terran::Core::UUID::invalid();
};

struct BoxCollider2DComponent {
    BoxCollider2DComponent() = default;

    glm::vec2 Offset = { 0.0f, 0.0f };
    glm::vec2 Size = { 1.0f, 1.0f };
    bool Sensor = false;

    uint32_t ColliderIndex = 0;
};

struct CircleCollider2DComponent {
    CircleCollider2DComponent() = default;

    glm::vec2 Offset = { 0.0f, 0.0f };
    float Radius = 0.5f;
    bool Sensor = false;

    uint32_t ColliderIndex = 0;
};

struct CapsuleCollider2DComponent {
    CapsuleCollider2DComponent() = default;

    glm::vec2 Offset = { 0.0f, 0.0f };
    glm::vec2 Size = { 0.5f, 1.0f };
    bool Sensor = false;

    uint32_t ColliderIndex = 0;
};

}
