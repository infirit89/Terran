#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "Image.h"

namespace Terran::Graphics {

enum class PresentMode {
    Mailbox,
    Fifo,
    Immediate
};

class LogicalDevice;
class Swapchain {
public:
    Swapchain(LogicalDevice* logicalDevice, uint32_t width, uint32_t height, PresentMode presentMode);
    ~Swapchain();

    VkExtent2D const& GetExtent() const;
    std::vector<VkImageView> const& GetImageViews() const;
    VkSurfaceFormatKHR const& GetSurfaceFormat() const;
    constexpr VkSwapchainKHR const& GetVulkanSwapchain() const;
    void Resize(uint32_t width, uint32_t height);
    void Present(VkSemaphore semaphore, uint32_t imageIndex);
    void AcquireNextImage(VkSemaphore imageAcquireSemaphore, uint32_t& imageIndex);
    constexpr Image* GetDepthImage() const
    {
        return m_DepthImage;
    }

private:
    void Create();
    void Destroy(VkSwapchainKHR swapchain);
    constexpr VkSurfaceFormatKHR const& ChooseCorrectSurfaceFormat(std::vector<VkSurfaceFormatKHR> const& surfaceFormats);
    constexpr VkPresentModeKHR ChooseSurfacePresentMode(std::vector<VkPresentModeKHR> const& presentModes);
    constexpr VkExtent2D ChooseSwapchainExtent(VkSurfaceCapabilitiesKHR const& surfaceCapabilities);
    void CreateDepthResources();

private:
    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    uint32_t m_Width, m_Height;
    PresentMode m_DesiredPresentMode;
    LogicalDevice* m_LogicalDevice;
    VkSurfaceFormatKHR m_SurfaceFormat;
    std::vector<VkImage> m_Images;
    std::vector<VkImageView> m_ImageViews;
    Image* m_DepthImage;
    VkExtent2D m_Extent;
};

}
