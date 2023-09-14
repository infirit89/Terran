#include "trpch.h"
#include "SerializerUtils.h"

namespace TerranEngine
{
    namespace SerializerUtils
    {
        nlohmann::ordered_json SerializeVec2(const glm::vec2& value) 
        {
            return
            {
                { "X", value.x },
                { "Y", value.y },
            };
        }

        glm::vec2 DeserializeVec2(nlohmann::ordered_json& j, const std::string& name)
        {
            glm::vec2 vec = glm::vec2(0.0f);

            vec.x = j[name]["X"];
            vec.y = j[name]["Y"];

            return vec;
        }

        nlohmann::ordered_json SerializeVec3(const glm::vec3& value)
        {
            return
            {
                { "X", value.x },
                { "Y", value.y },
                { "Z", value.z }
            };
        }

        glm::vec3 DeserializeVec3(nlohmann::ordered_json& j, const std::string& name) 
        {
            glm::vec3 vec = glm::vec3(0.0f);

            vec.x = j[name]["X"];
            vec.y = j[name]["Y"];
            vec.z = j[name]["Z"];

            return vec;
        }

        nlohmann::ordered_json SerializeVec4(const glm::vec4& value)
        {
            return
            {
                { "X", value.x },
                { "Y", value.y },
                { "Z", value.z },
                { "W", value.w }
            };
        }


        glm::vec4 DeserializeVec4(nlohmann::ordered_json& j, const std::string& name)
        {
            glm::vec4 vec = glm::vec4(0.0f);

            vec.x = j[name]["X"];
            vec.y = j[name]["Y"];
            vec.z = j[name]["Z"];
            vec.w = j[name]["W"];

            return vec;
        }

    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const UUID& v) 
    {
        out << std::to_string(v);
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const TextureFilter& v)
    {
        out << TextureFilterToString(v);
        return out;
    }
}
