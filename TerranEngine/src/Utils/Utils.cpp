#include "trpch.h"
#include "Utils.h"

namespace TerranEngine 
{
    glm::vec4 Utils::NormalizeColor4(const glm::vec4& color)
    {
        return { color.x / 255, color.y / 255, color.z / 255, color.w / 255 };
    }
}

