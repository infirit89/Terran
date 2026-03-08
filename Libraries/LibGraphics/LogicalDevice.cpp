#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "vulkan/vulkan_core.h"

#include <cassert>
#include <cstdint>

namespace Terran::Graphics {

LogicalDevice::LogicalDevice(VkDevice device, PhysicalDevice* physicalDevice)
    : m_LogicalDevice(device)
    , m_PhysicalDevice(physicalDevice)
{
    auto const& queueFamilyIndices = m_PhysicalDevice->GetQueueFamilyIndices();

    assert(queueFamilyIndices.GraphicsFamily.has_value());
    assert(queueFamilyIndices.PresentationFamily.has_value());

    uint32_t transferQueueFamilyIndex = queueFamilyIndices.TransferFamily.has_value() ? queueFamilyIndices.TransferFamily.value() : queueFamilyIndices.GraphicsFamily.value();
    vkGetDeviceQueue(device, queueFamilyIndices.GraphicsFamily.value(), 0, &m_GraphicsQueue);
    vkGetDeviceQueue(device, queueFamilyIndices.PresentationFamily.value(), 0, &m_PresentQueue);
    vkGetDeviceQueue(device, transferQueueFamilyIndex, 0, &m_TransferQueue);
}

LogicalDevice::~LogicalDevice()
{
    vkDestroyDevice(m_LogicalDevice, nullptr);
}

void LogicalDevice::WaitIdle() const
{
    vkDeviceWaitIdle(m_LogicalDevice);
}

void LogicalDevice::QueueSubmit(VkQueue queue, uint32_t submitCount, VkSubmitInfo* submitInfos, VkFence fence)
{
    assert(vkQueueSubmit(queue, submitCount, submitInfos, fence) == VK_SUCCESS);
}

void LogicalDevice::QueueWaitIdle(VkQueue queue)
{
    assert(vkQueueWaitIdle(queue) == VK_SUCCESS);
}

void LogicalDevice::SubmitImmediateCommands(CommandBuffer const& commandBuffer, VkQueue queue)
{
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer.GetVulkanCommandBuffer();

    QueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    QueueWaitIdle(queue);
}

}
