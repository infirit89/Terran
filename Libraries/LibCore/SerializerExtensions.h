#pragma once

#include "UUID.h"

#include <glm/glm.hpp>

#include <yaml-cpp/yaml.h>

namespace Terran::Core {

struct Vec2ToYaml {
    explicit Vec2ToYaml(glm::vec2 const& vec)
        : data(vec)
    {
    }
    friend YAML::Emitter& operator<<(YAML::Emitter& out, Vec2ToYaml const& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.data.x << v.data.y << YAML::EndSeq;
        return out;
    }

    glm::vec2 const& data;
};

struct Vec3ToYaml {

    explicit Vec3ToYaml(glm::vec3 const& vec)
        : data(vec)
    {
    }
    friend YAML::Emitter& operator<<(YAML::Emitter& out, Vec3ToYaml const& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.data.x << v.data.y << v.data.z << YAML::EndSeq;
        return out;
    }

    glm::vec3 const& data;
};

struct Vec4ToYaml {

    explicit Vec4ToYaml(glm::vec4 const& vec)
        : data(vec)
    {
    }
    friend YAML::Emitter& operator<<(YAML::Emitter& out, Vec4ToYaml const& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.data.x << v.data.y << v.data.z << v.data.w << YAML::EndSeq;
        return out;
    }

    glm::vec4 data;
};

struct UUIDToYaml {

    explicit UUIDToYaml(Core::UUID const& id)
        : data(id)
    {
    }
    friend YAML::Emitter& operator<<(YAML::Emitter& out, UUIDToYaml const& id)
    {
        out << std::to_string(id.data);
        return out;
    }

    Core::UUID const& data;
};
struct ByteToYaml {

    explicit ByteToYaml(std::byte const& byte)
        : data(byte)
    {
    }

    friend YAML::Emitter& operator<<(YAML::Emitter& out, ByteToYaml const& byte)
    {
        out << static_cast<int>(byte.data);
        return out;
    }

    std::byte const& data;
};

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
