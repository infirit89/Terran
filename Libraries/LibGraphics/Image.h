#pragma once

#include "vulkan/vulkan.h"

namespace Terran::Graphics {

struct ImageCreateInfo {
    uint32_t Width, Height;
    VkFormat Format;
    VkImageTiling Tiling;
    VkImageUsageFlags Usage;
    VkMemoryPropertyFlags MemoryProperties;
    VkImageAspectFlags AspectFlags;
};

class Image {
public:
    Image(ImageCreateInfo const& imageCreateInfo);
    ~Image();

    Image(Image const& other) = delete;
    Image(Image&& other) = delete;
    Image& operator=(Image const& other) = delete;

    VkImage const& GetVulkanImage() const { return m_Image; }
    VkImageView const& GetVulkanImageView() const
    {
        return m_ImageView;
    }

    VkDeviceMemory const& GetVulkanImageMemory() const
    {
        return m_ImageMemory;
    }

    VkImageLayout const& GetCurrentVulkanLayout() const
    {
        return m_CurrentLayout;
    }

    VkFormat const& GetFormat() const
    {
        return m_Format;
    }

private:
    void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling imageTiling, VkImageUsageFlags imageUsage, VkMemoryPropertyFlags memoryProperties);
    void CreateView(VkFormat imageFormat, VkImageAspectFlags imageAspect);

private:
    VkImage m_Image;
    VkImageView m_ImageView;
    VkDeviceMemory m_ImageMemory;
    uint32_t m_Width, m_Height;
    VkImageLayout m_CurrentLayout;
    VkFormat m_Format;

    friend class CommandBuffer;
};

}
