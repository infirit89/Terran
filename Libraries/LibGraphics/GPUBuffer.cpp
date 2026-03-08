#include "GPUBuffer.h"
#include "RendererContext.h"

namespace Terran::Graphics {

GPUBuffer::GPUBuffer(VkBufferUsageFlags buffer_usage, VkDeviceSize buffer_size,
    VkMemoryPropertyFlags memory_properties)
    : m_BufferSize(buffer_size)
{
    Create(buffer_usage, buffer_size, memory_properties);
}

GPUBuffer::~GPUBuffer()
{
    LogicalDevice* logicalDevice = RendererContext::GetLogicalDevice();
    vkDestroyBuffer(logicalDevice->GetVulkanDevice(), m_Buffer, nullptr);
    vkFreeMemory(logicalDevice->GetVulkanDevice(), m_BufferMemory, nullptr);
}

void* GPUBuffer::MapMemory()
{
    void* data;
    LogicalDevice* logicalDevice = RendererContext::GetLogicalDevice();
    assert(vkMapMemory(logicalDevice->GetVulkanDevice(), m_BufferMemory, 0,
               m_BufferSize, 0, &data)
        == VK_SUCCESS);
    return data;
}

void GPUBuffer::UnmapMemory()
{
    LogicalDevice* logicalDevice = RendererContext::GetLogicalDevice();
    vkUnmapMemory(logicalDevice->GetVulkanDevice(), m_BufferMemory);
}

void GPUBuffer::Create(VkBufferUsageFlags usage, VkDeviceSize size,
    VkMemoryPropertyFlags memoryProperties)
{
    LogicalDevice* logicalDevice = RendererContext::GetLogicalDevice();
    QueueFamilyIndices const& queueFamilyIndices = logicalDevice->GetPhysicalDevice()->GetQueueFamilyIndices();

    size_t queueFamilyIndexCount = queueFamilyIndices.TransferFamily.has_value() ? 2 : 1;
    std::array queueFamilyArr {
        queueFamilyIndices.GraphicsFamily.value(),
        queueFamilyIndices.TransferFamily.value_or(0),
    };

    VkBufferCreateInfo bufferCreateInfo {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = size;
    bufferCreateInfo.queueFamilyIndexCount = queueFamilyIndexCount;
    bufferCreateInfo.pQueueFamilyIndices = queueFamilyArr.data();
    bufferCreateInfo.sharingMode = queueFamilyIndices.GraphicsFamily.value() != queueFamilyIndices.TransferFamily.value_or(queueFamilyIndices.GraphicsFamily.value())
        ? VK_SHARING_MODE_CONCURRENT
        : VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.usage = usage;

    assert(vkCreateBuffer(logicalDevice->GetVulkanDevice(), &bufferCreateInfo,
               nullptr, &m_Buffer)
        == VK_SUCCESS);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(logicalDevice->GetVulkanDevice(), m_Buffer,
        &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = logicalDevice->GetPhysicalDevice()->FindMemoryType(
        memoryRequirements.memoryTypeBits, memoryProperties);

    assert(vkAllocateMemory(logicalDevice->GetVulkanDevice(), &memoryAllocateInfo,
               nullptr, &m_BufferMemory)
        == VK_SUCCESS);

    assert(vkBindBufferMemory(logicalDevice->GetVulkanDevice(), m_Buffer,
               m_BufferMemory, 0)
        == VK_SUCCESS);
}

} // namespace LearningVulkan
