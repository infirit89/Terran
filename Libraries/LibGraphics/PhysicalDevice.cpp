#include "PhysicalDevice.h"
#include "RendererContext.h"
#include "VulkanUtils.h"

#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace Terran::Graphics {

PhysicalDevice::PhysicalDevice(VkPhysicalDevice physicalDevice)
    : m_PhysicalDevice(physicalDevice)
{
}

LogicalDevice* PhysicalDevice::CreateLogicalDevice()
{
    m_QueueFamilyIndices = FindQueueFamilyIndices(m_PhysicalDevice);

    VkDeviceQueueCreateInfo graphicsQueueCreateInfo {};
    graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsQueueCreateInfo.queueFamilyIndex = m_QueueFamilyIndices.GraphicsFamily.value();
    graphicsQueueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceQueueCreateInfo transferQueueCreateInfo {};
    transferQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    transferQueueCreateInfo.queueFamilyIndex = m_QueueFamilyIndices.TransferFamily.value_or(0);
    transferQueueCreateInfo.queueCount = 1;
    transferQueueCreateInfo.pQueuePriorities = &queuePriority;

    size_t queueCreateInfosSize = m_QueueFamilyIndices.TransferFamily.has_value() ? 2 : 1;
    std::array queueCreateInfos = {
        graphicsQueueCreateInfo,
        transferQueueCreateInfo,
    };

    VkDeviceCreateInfo deviceCreateInfo {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceCreateInfo.enabledLayerCount = VulkanUtils::LayerCount;
    deviceCreateInfo.ppEnabledLayerNames = VulkanUtils::Layers.data();

    deviceCreateInfo.queueCreateInfoCount = queueCreateInfosSize;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    deviceCreateInfo.enabledExtensionCount = VulkanUtils::DeviceExtensionsSize;
    deviceCreateInfo.ppEnabledExtensionNames = VulkanUtils::DeviceExtensions.data();

    VkPhysicalDeviceFeatures features {};
    features.samplerAnisotropy = VK_TRUE;

    deviceCreateInfo.pEnabledFeatures = &features;

    VkDevice device;
    assert(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr,
               &device)
        == VK_SUCCESS);
    return new LogicalDevice(device, this);
}

QueueFamilyIndices
PhysicalDevice::FindQueueFamilyIndices(VkPhysicalDevice physicalDevice)
{
    QueueFamilyIndices queueFamilyIndices;
    uint32_t queueFamilyPropertiesCount;
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice, &queueFamilyPropertiesCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(
        queueFamilyPropertiesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
        &queueFamilyPropertiesCount,
        queueFamilyProperties.data());

    uint32_t index = 0;
    std::cout << queueFamilyPropertiesCount << '\n';
    for (auto const& properties : queueFamilyProperties) {
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {

            std::cout << "supports graphics queues" << '\n';
            queueFamilyIndices.GraphicsFamily = index;
        } else if (properties.queueFlags & VK_QUEUE_TRANSFER_BIT) {

            std::cout << "supports transfer queues" << '\n';
            queueFamilyIndices.TransferFamily = index;
        }

        VkBool32 presentationSupported;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index,
            RendererContext::GetVulkanSurface(),
            &presentationSupported);
        if (presentationSupported) {
            std::cout << "supports presentation" << '\n';
            queueFamilyIndices.PresentationFamily = index;
        }

        if (queueFamilyIndices.IsComplete())
            break;

        index++;
    }

    return queueFamilyIndices;
}

uint32_t
PhysicalDevice::RateDeviceSuitability(VkPhysicalDevice physicalDevice)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    uint32_t score = 1;

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score++;

    QueueFamilyIndices const& queueFamilyIndices = FindQueueFamilyIndices(physicalDevice);
    if (!queueFamilyIndices.IsComplete()) {
        std::cout << "incomplete family indices" << '\n';
        score = 0;
    }

    if (!CheckDeviceExtensionSupport(physicalDevice)) {

        std::cout << "doesn't support required extensions" << '\n';
        score = 0;
    }

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
    if (deviceFeatures.samplerAnisotropy == VK_FALSE) {

        std::cout << "doesn't support sampler anisotropy" << '\n';
        score = 0;
    }

    std::cout << '\t' << "Name: " << deviceProperties.deviceName
              << "; Score: " << score << '\n';
    return score;
}

bool PhysicalDevice::CheckDeviceExtensionSupport(
    VkPhysicalDevice physicalDevice)
{
    uint32_t extensionCount = 0;

    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
        nullptr);

    std::vector<VkExtensionProperties> extensionProperties(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
        extensionProperties.data());

    std::set<std::string> requiredExtensions(
        VulkanUtils::DeviceExtensions.begin(),
        VulkanUtils::DeviceExtensions.end());

    for (auto const& extension : extensionProperties)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

SwapchainSupportDetails PhysicalDevice::QuerySwapChainSupport()
{
    SwapchainSupportDetails swapChainSupport;

    VkSurfaceKHR surface = RendererContext::GetVulkanSurface();

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        m_PhysicalDevice, surface, &swapChainSupport.SurfaceCapabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, surface, &formatCount,
        nullptr);

    if (formatCount > 0) {
        swapChainSupport.SurfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            m_PhysicalDevice, surface, &formatCount,
            swapChainSupport.SurfaceFormats.data());
    }

    uint32_t presentModesCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, surface,
        &presentModesCount, nullptr);

    if (presentModesCount > 0) {
        swapChainSupport.PresentModes.resize(presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            m_PhysicalDevice, surface, &presentModesCount,
            swapChainSupport.PresentModes.data());
    }

    return swapChainSupport;
}

uint32_t PhysicalDevice::FindMemoryType(uint32_t memoryMask,
    VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        if (memoryMask & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    assert(false);
    return 0;
}

PhysicalDevice* PhysicalDevice::GetSuitablePhysicalDevice()
{
    uint32_t physicalDeviceCount = 0;
    VkInstance instance = RendererContext::GetVulkanInstance();

    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    assert(physicalDeviceCount > 0);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
        physicalDevices.data());

    std::multimap<uint32_t, VkPhysicalDevice> deviceCandidates;

    std::cout << "Available devices:\n";
    for (auto const& physicalDevice : physicalDevices) {
        uint32_t score = RateDeviceSuitability(physicalDevice);
        deviceCandidates.insert(std::make_pair(score, physicalDevice));
    }

    if (deviceCandidates.rbegin()->first > 0) {
        VkPhysicalDevice vulkanPhysicalDevice = deviceCandidates.rbegin()->second;
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(vulkanPhysicalDevice, &deviceProperties);
        std::cout << "Picked device: " << deviceProperties.deviceName << '\n';

        PhysicalDevice* physicalDevice = new PhysicalDevice(vulkanPhysicalDevice);

        return physicalDevice;
    } else {
        std::cerr << "No suitable device found" << '\n';
        return nullptr;
    }
}

} // namespace LearningVulkan
