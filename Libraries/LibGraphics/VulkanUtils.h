#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h>
#include <array>

namespace Terran::Graphics 
{
    namespace VulkanUtils 
    {
        inline constexpr size_t LayerCount = 1;
        inline std::array<const char*, LayerCount> Layers
        {
            "VK_LAYER_KHRONOS_validation",
        };

        inline constexpr size_t DeviceExtensionsSize = 2;
        inline std::array<const char*, DeviceExtensionsSize> DeviceExtensions
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
        };
    }
}
