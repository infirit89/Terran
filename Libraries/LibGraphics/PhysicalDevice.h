#pragma once

#include "LogicalDevice.h"

#include "vulkan/vulkan.h"

#include <cstdint>
#include <optional>
#include <vector>

namespace Terran::Graphics {

struct QueueFamilyIndices {
    std::optional<uint32_t> GraphicsFamily, PresentationFamily, TransferFamily;
    bool IsComplete() const { return GraphicsFamily.has_value() && PresentationFamily.has_value(); }
};

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR SurfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> SurfaceFormats;
    std::vector<VkPresentModeKHR> PresentModes;
};

class PhysicalDevice {
public:
    PhysicalDevice() = delete;
    ~PhysicalDevice() = default;

    static PhysicalDevice* GetSuitablePhysicalDevice();

    QueueFamilyIndices const& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
    VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }

    LogicalDevice* CreateLogicalDevice();
    SwapchainSupportDetails QuerySwapChainSupport();
    uint32_t FindMemoryType(uint32_t memoryMask, VkMemoryPropertyFlags properties);

private:
    PhysicalDevice(VkPhysicalDevice physicalDevice);
    static QueueFamilyIndices FindQueueFamilyIndices(VkPhysicalDevice physicalDevice);
    static uint32_t RateDeviceSuitability(VkPhysicalDevice physicalDevice);
    static bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

private:
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    QueueFamilyIndices m_QueueFamilyIndices;
};

}
