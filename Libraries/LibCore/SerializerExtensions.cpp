#include "SerializerExtensions.h"

namespace Terran::Core {

YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec2 const& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec3 const& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, glm::vec4 const& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, Terran::Core::UUID const& v)
{
    out << std::to_string(v);
    return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, std::byte v)
{
    out << static_cast<int>(v);
    return out;
}

}
