#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <array>

namespace Terran::Graphics
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Color;
        glm::vec2 TextureCoordinates;

        static VkVertexInputBindingDescription GetBindingDescription()
        {
            return {
                .binding = 0,
                .stride = sizeof(Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
            };
        }

        static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
        {
            std::array attributeDescriptions = {
                VkVertexInputAttributeDescription { .location = 0, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(Vertex, Position) },
                VkVertexInputAttributeDescription { .location = 1, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(Vertex, Color) },
                VkVertexInputAttributeDescription {.location = 2, .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT, .offset = offsetof(Vertex, TextureCoordinates) },
            };

            return attributeDescriptions;
        }
    };

    struct CameraData
    {
        glm::mat4 Projection;
        glm::mat4 View;
    };

    struct LightData
    {
        glm::vec3 LightColor;
    };
}
