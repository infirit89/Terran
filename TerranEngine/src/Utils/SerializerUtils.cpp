#include "trpch.h"
#include "SerializerUtils.h"

namespace TerranEngine
{
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

    YAML::Emitter& operator<<(YAML::Emitter& out, std::byte v)
    {
        out << static_cast<int>(v);
        return out;
    }
}
