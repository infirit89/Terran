#include "Swapchain.h"
#include "LogicalDevice.h"
#include "RendererContext.h"

#include <algorithm>
#include <cassert>
#include <iostream>

namespace Terran::Graphics {

static constexpr VkPresentModeKHR ConvertToVkPresentMode(PresentMode presentMode)
{
    switch (presentMode) {
    case PresentMode::Mailbox:
        return VK_PRESENT_MODE_MAILBOX_KHR;
    case PresentMode::Fifo:
        return VK_PRESENT_MODE_FIFO_KHR;
    case PresentMode::Immediate:
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    std::cerr << "Couldn't convert to vk present mode defaulting to fifo\n";
    return VK_PRESENT_MODE_FIFO_KHR;
}

Swapchain::Swapchain(LogicalDevice* logicalDevice, uint32_t width, uint32_t height, PresentMode presentMode)
    : m_Width(width)
    , m_Height(height)
    , m_DesiredPresentMode(presentMode)
    , m_LogicalDevice(logicalDevice)
{
    Create();
}

Swapchain::~Swapchain()
{
    Destroy(m_Swapchain);
}

VkExtent2D const& Swapchain::GetExtent() const
{
    return m_Extent;
}

std::vector<VkImageView> const& Swapchain::GetImageViews() const
{
    return m_ImageViews;
}

VkSurfaceFormatKHR const& Swapchain::GetSurfaceFormat() const
{
    return m_SurfaceFormat;
}

constexpr VkSwapchainKHR const& Swapchain::GetVulkanSwapchain() const
{
    return m_Swapchain;
}

void Swapchain::Resize(uint32_t width, uint32_t height)
{
    m_Width = width;
    m_Height = height;
    Create();
}

void Swapchain::Present(VkSemaphore semaphore, uint32_t imageIndex)
{
    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &semaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_Swapchain;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(m_LogicalDevice->GetPresentQueue(), &presentInfo);
}

void Swapchain::AcquireNextImage(VkSemaphore imageAcquireSemaphore, uint32_t& imageIndex)
{
    vkAcquireNextImageKHR(m_LogicalDevice->GetVulkanDevice(), m_Swapchain, UINT64_MAX,
        imageAcquireSemaphore, VK_NULL_HANDLE, &imageIndex);
}

void Swapchain::Create()
{
    SwapchainSupportDetails const& swapchainDetails = m_LogicalDevice->GetPhysicalDevice()->QuerySwapChainSupport();
    VkPresentModeKHR presentMode = ChooseSurfacePresentMode(swapchainDetails.PresentModes);
    m_Extent = ChooseSwapchainExtent(swapchainDetails.SurfaceCapabilities);
    m_SurfaceFormat = ChooseCorrectSurfaceFormat(swapchainDetails.SurfaceFormats);

    VkSurfaceCapabilitiesKHR const& surfaceCapabilities = swapchainDetails.SurfaceCapabilities;
    uint32_t minImageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount)
        minImageCount = surfaceCapabilities.maxImageCount;

    VkSwapchainKHR oldSwapchain = m_Swapchain;

    VkSwapchainCreateInfoKHR swapchainCreateInfo {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.imageExtent = m_Extent;
    swapchainCreateInfo.surface = RendererContext::GetVulkanSurface();
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.imageFormat = m_SurfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.minImageCount = minImageCount;

    QueueFamilyIndices const& queueFamilyIndices = m_LogicalDevice->GetPhysicalDevice()->GetQueueFamilyIndices();

    uint32_t indices[] = { queueFamilyIndices.GraphicsFamily.value(), queueFamilyIndices.PresentationFamily.value() };

    if (queueFamilyIndices.GraphicsFamily != queueFamilyIndices.PresentationFamily) {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = indices;
    } else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    swapchainCreateInfo.preTransform = swapchainDetails.SurfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = oldSwapchain;

    assert(vkCreateSwapchainKHR(m_LogicalDevice->GetVulkanDevice(), &swapchainCreateInfo, nullptr, &m_Swapchain) == VK_SUCCESS);

    if (oldSwapchain != VK_NULL_HANDLE)
        Destroy(oldSwapchain);

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(m_LogicalDevice->GetVulkanDevice(), m_Swapchain, &imageCount, nullptr);
    m_Images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_LogicalDevice->GetVulkanDevice(), m_Swapchain, &imageCount, m_Images.data());

    m_ImageViews.resize(imageCount);

    for (size_t i = 0; i < m_Images.size(); i++) {
        VkImageViewCreateInfo imageViewCreateInfo {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = m_Images.at(i);
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = m_SurfaceFormat.format;
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        assert(vkCreateImageView(m_LogicalDevice->GetVulkanDevice(), &imageViewCreateInfo, nullptr, &m_ImageViews.at(i)) == VK_SUCCESS);
    }

    CreateDepthResources();
}

void Swapchain::Destroy(VkSwapchainKHR swapchain)
{
    for (auto const& imageViews : m_ImageViews)
        vkDestroyImageView(m_LogicalDevice->GetVulkanDevice(), imageViews, nullptr);

    vkDestroySwapchainKHR(m_LogicalDevice->GetVulkanDevice(), swapchain, nullptr);

    delete m_DepthImage;
}

void Swapchain::CreateDepthResources()
{
    std::array desiredDepthFormats = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
    };

    VkFormat depthFormat = VK_FORMAT_UNDEFINED;
    PhysicalDevice* physicalDevice = m_LogicalDevice->GetPhysicalDevice();
    for (auto desiredDepthFormat : desiredDepthFormats) {
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice->GetPhysicalDevice(), desiredDepthFormat, &format_properties);

        if ((format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            depthFormat = desiredDepthFormat;
            break;
        }
        /*else if((format_properties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            depth_format = desired_depth_format;
            break;
        }*/
    }

    assert(depthFormat != VK_FORMAT_UNDEFINED);

    auto swapchainExtent = GetExtent();
    ImageCreateInfo depthImageCreateInfo;
    depthImageCreateInfo.Width = swapchainExtent.width;
    depthImageCreateInfo.Height = swapchainExtent.height;
    depthImageCreateInfo.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthImageCreateInfo.Tiling = VK_IMAGE_TILING_OPTIMAL;
    depthImageCreateInfo.Format = depthFormat;
    depthImageCreateInfo.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    depthImageCreateInfo.AspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;

    m_DepthImage = new Image(depthImageCreateInfo);
}

constexpr VkSurfaceFormatKHR const& Swapchain::ChooseCorrectSurfaceFormat(std::vector<VkSurfaceFormatKHR> const& surfaceFormats)
{
    for (auto const& surfaceFormat : surfaceFormats) {
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
            return surfaceFormat;
    }

    return surfaceFormats.at(0);
}

constexpr VkPresentModeKHR Swapchain::ChooseSurfacePresentMode(std::vector<VkPresentModeKHR> const& presentModes)
{
    VkPresentModeKHR desiredPresentMode = ConvertToVkPresentMode(m_DesiredPresentMode);
    for (auto const& presentMode : presentModes) {
        // present images last in first out
        if (presentMode == desiredPresentMode)
            return presentMode;
    }

    // present images first in first out
    return VK_PRESENT_MODE_FIFO_KHR;
}

constexpr VkExtent2D Swapchain::ChooseSwapchainExtent(VkSurfaceCapabilitiesKHR const& surfaceCapabilities)
{
    VkExtent2D extent = {
        m_Width,
        m_Height
    };

    extent.width = std::clamp(extent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

    return extent;
}

}
