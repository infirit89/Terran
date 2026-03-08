#include "Image.h"
#include "LogicalDevice.h"
#include "RendererContext.h"
#include "vulkan/vulkan_core.h"

#include <vulkan/vulkan.h>

namespace Terran::Graphics {
Image::Image(const ImageCreateInfo &imageCreateInfo)
    : m_Width(imageCreateInfo.Width), m_Height(imageCreateInfo.Height),
      m_Format(imageCreateInfo.Format),
      m_CurrentLayout(VK_IMAGE_LAYOUT_UNDEFINED) {
  CreateImage(m_Width, m_Height, m_Format, imageCreateInfo.Tiling,
              imageCreateInfo.Usage, imageCreateInfo.MemoryProperties);

  CreateView(m_Format, imageCreateInfo.AspectFlags);
}

Image::~Image() {
  LogicalDevice *logicalDevice = RendererContext::GetLogicalDevice();
  vkDestroyImageView(logicalDevice->GetVulkanDevice(), m_ImageView, nullptr);
  vkDestroyImage(logicalDevice->GetVulkanDevice(), m_Image, nullptr);
  vkFreeMemory(logicalDevice->GetVulkanDevice(), m_ImageMemory, nullptr);
}

void Image::CreateImage(uint32_t width, uint32_t height, VkFormat format,
                        VkImageTiling imageTiling, VkImageUsageFlags imageUsage,
                        VkMemoryPropertyFlags memoryProperties) {
  LogicalDevice *logicalDevice = RendererContext::GetLogicalDevice();
  PhysicalDevice *physicalDevice = logicalDevice->GetPhysicalDevice();
  const auto &queueFamilyIndices = physicalDevice->GetQueueFamilyIndices();
  VkImageCreateInfo imageCreateInfo{};

  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.format = format;
  imageCreateInfo.usage = imageUsage;
  imageCreateInfo.extent.width = width;
  imageCreateInfo.extent.height = height;
  imageCreateInfo.extent.depth = 1;
  imageCreateInfo.tiling = imageTiling;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.mipLevels = 1;
  imageCreateInfo.arrayLayers = 1;
  imageCreateInfo.sharingMode = queueFamilyIndices.TransferFamily.has_value() ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
  imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;


  size_t queueFamilyIndexCount =
      queueFamilyIndices.TransferFamily.has_value() ? 2 : 1;
  std::array queueFamilyIndicesArr = {
      queueFamilyIndices.GraphicsFamily.value(),
      queueFamilyIndices.TransferFamily.value_or(0),
  };

  imageCreateInfo.queueFamilyIndexCount = queueFamilyIndexCount;
  imageCreateInfo.pQueueFamilyIndices = queueFamilyIndicesArr.data();

  assert(vkCreateImage(logicalDevice->GetVulkanDevice(), &imageCreateInfo,
                       nullptr, &m_Image) == VK_SUCCESS);

  VkMemoryRequirements imageMemoryRequirements;
  vkGetImageMemoryRequirements(logicalDevice->GetVulkanDevice(), m_Image,
                               &imageMemoryRequirements);

  VkMemoryAllocateInfo imageMemoryAllocateInfo{};
  imageMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  imageMemoryAllocateInfo.allocationSize = imageMemoryRequirements.size;
  imageMemoryAllocateInfo.memoryTypeIndex = physicalDevice->FindMemoryType(
      imageMemoryRequirements.memoryTypeBits, memoryProperties);

  assert(vkAllocateMemory(logicalDevice->GetVulkanDevice(),
                          &imageMemoryAllocateInfo, nullptr,
                          &m_ImageMemory) == VK_SUCCESS);

  assert(vkBindImageMemory(logicalDevice->GetVulkanDevice(), m_Image,
                           m_ImageMemory, 0) == VK_SUCCESS);
}

void Image::CreateView(VkFormat imageFormat, VkImageAspectFlags imageAspect) {
  VkImageViewCreateInfo imageViewCreateInfo{};
  imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewCreateInfo.image = m_Image;
  imageViewCreateInfo.format = imageFormat;
  imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.subresourceRange.aspectMask = imageAspect;
  imageViewCreateInfo.subresourceRange.layerCount = 1;
  imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
  imageViewCreateInfo.subresourceRange.levelCount = 1;
  imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;

  LogicalDevice *logicalDevice = RendererContext::GetLogicalDevice();

  assert(vkCreateImageView(logicalDevice->GetVulkanDevice(),
                           &imageViewCreateInfo, nullptr,
                           &m_ImageView) == VK_SUCCESS);
}
} // namespace LearningVulkan
