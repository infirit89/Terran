#pragma once

#include "LibCore/UUID.h"

#include "Graphics/Texture.h"

#include <glm/glm.hpp>

#include <yaml-cpp/yaml.h>

namespace TerranEngine {

YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec2 const& v);
YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec3 const& v);
YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec4 const& v);
YAML::Emitter& operator<<(YAML::Emitter& out, Terran::Core::UUID const& v);
YAML::Emitter& operator<<(YAML::Emitter& out, TextureFilter const& v);
YAML::Emitter& operator<<(YAML::Emitter& out, std::byte v);

}

namespace YAML {

template<>
struct convert<glm::vec2> {
    static Node encode(glm::vec2 const& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(Node const& node, glm::vec2& rhs)
    {
        if (!node.IsSequence() || node.size() != 2) {
            return false;
        }

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        return true;
    }
};

template<>
struct convert<glm::vec3> {
    static Node encode(glm::vec3 const& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(Node const& node, glm::vec3& rhs)
    {
        if (!node.IsSequence() || node.size() != 3) {
            return false;
        }

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }
};

template<>
struct convert<glm::vec4> {
    static Node encode(glm::vec4 const& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.push_back(rhs.w);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(Node const& node, glm::vec4& rhs)
    {
        if (!node.IsSequence() || node.size() != 4) {
            return false;
        }

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        rhs.w = node[3].as<float>();
        return true;
    }
};

template<>
struct convert<Terran::Core::UUID> {
    static Node encode(Terran::Core::UUID const& rhs)
    {
        Node node;
        node.push_back(std::to_string(rhs));
        return node;
    }

    static bool decode(Node const& node, Terran::Core::UUID& rhs)
    {

        rhs = Terran::Core::UUID::from_string(node.as<std::string>());
        return true;
    }
};

template<>
struct convert<TerranEngine::TextureFilter> {
    static Node encode(TerranEngine::TextureFilter const& rhs)
    {
        Node node;
        node.push_back(TerranEngine::TextureFilterToString(rhs));
        return node;
    }

    static bool decode(Node const& node, TerranEngine::TextureFilter& rhs)
    {

        rhs = TerranEngine::TextureFilterFromString(node.as<std::string>());
        return true;
    }
};

template<>
struct convert<std::byte> {
    static Node encode(std::byte rhs)
    {
        Node node;
        node.push_back(static_cast<int>(rhs));
        return node;
    }
    static bool decode(Node const& node, std::byte& rhs)
    {
        rhs = static_cast<std::byte>(node.as<int32_t>());
        return true;
    }
};

}
