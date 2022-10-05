#pragma once

#include <glm/glm.hpp>
#include <json.hpp>

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
}
