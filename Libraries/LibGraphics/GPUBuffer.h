#pragma once

#include "vulkan/vulkan.h"

namespace Terran::Graphics
{
    class GPUBuffer
    {
    public:
        GPUBuffer(VkBufferUsageFlags buffer_usage, VkDeviceSize buffer_size, VkMemoryPropertyFlags memory_properties);
        ~GPUBuffer();
        GPUBuffer(const GPUBuffer& other) = delete;
        GPUBuffer(GPUBuffer&& other) = delete;

        GPUBuffer& operator=(const GPUBuffer& other) = delete;


        VkBuffer GetVulkanBuffer() const { return m_Buffer; }
        VkDeviceMemory GetVulkanBufferMemory() const { return m_BufferMemory; }
        void* MapMemory();
        void UnmapMemory();

        static void Copy(const GPUBuffer*, const GPUBuffer*, VkDeviceSize size);

    private:
        void Create(VkBufferUsageFlags usage, VkDeviceSize size, VkMemoryPropertyFlags memoryProperties);

    private:
        VkBuffer m_Buffer;
        VkDeviceMemory m_BufferMemory;
        VkDeviceSize m_BufferSize;
    };
}
