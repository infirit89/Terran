#pragma once

#include "Core/UUID.h"

#include "Graphics/Texture.h"

#include <glm/glm.hpp>
#include <json.hpp>

#include <yaml-cpp/yaml.h>

namespace TerranEngine
{
    namespace SerializerUtils
    {
        nlohmann::ordered_json SerializeVec2(const glm::vec2& value);
        glm::vec2 DeserializeVec2(nlohmann::ordered_json& j, const std::string& name);
        nlohmann::ordered_json SerializeVec3(const glm::vec3& value);
        glm::vec3 DeserializeVec3(nlohmann::ordered_json& j, const std::string& name);
        nlohmann::ordered_json SerializeVec4(const glm::vec4& value);
        glm::vec4 DeserializeVec4(nlohmann::ordered_json& j, const std::string& name);
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v);
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v);
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v);
    YAML::Emitter& operator<<(YAML::Emitter& out, const UUID& v);
    YAML::Emitter& operator<<(YAML::Emitter& out, const TextureFilter& v);
}

namespace YAML {

    template<>
    struct convert<glm::vec2> {
        static Node encode(const glm::vec2& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs) {
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
        static Node encode(const glm::vec3& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs) {
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
        static Node encode(const glm::vec4& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs) {
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
    struct convert<TerranEngine::UUID> {
        static Node encode(const TerranEngine::UUID& rhs) {
            Node node;
            node.push_back(std::to_string(rhs));
            return node;
        }

        static bool decode(const Node& node, TerranEngine::UUID& rhs) {
            
            rhs = TerranEngine::UUID::FromString(node.as<std::string>());
            return true;
        }
    };

    template<>
        struct convert<TerranEngine::TextureFilter> {
        static Node encode(const TerranEngine::TextureFilter& rhs) {
            Node node;
            node.push_back(TerranEngine::TextureFilterToString(rhs));
            return node;
        }

        static bool decode(const Node& node, TerranEngine::TextureFilter& rhs) {

            rhs = TerranEngine::TextureFilterFromString(node.as<std::string>());
            return true;
        }
    };
}
