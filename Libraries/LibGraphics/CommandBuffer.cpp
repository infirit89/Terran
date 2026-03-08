#include "CommandBuffer.h"

#include "LogicalDevice.h"
#include "RendererContext.h"

namespace Terran::Graphics {

CommandBuffer::CommandBuffer(VkCommandPool const& commandPool,
    VkCommandBuffer&& commandBuffer)
    : m_CommandBuffer(commandBuffer)
    , m_CommandPool(commandPool)
{
}

/*CommandBuffer::CommandBuffer(const VkCommandPool& commandPool,
    VkCommandBufferLevel commandBufferLevel)
    : m_CommandPool(commandPool)
{
    AllocateCommandBuffer(commandPool, commandBufferLevel);
}*/

CommandBuffer::~CommandBuffer()
{
    LogicalDevice* logicalDevice = RendererContext::GetLogicalDevice();
    vkFreeCommandBuffers(logicalDevice->GetVulkanDevice(), m_CommandPool,
        1, &m_CommandBuffer);
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    : m_CommandBuffer(std::move(other.m_CommandBuffer))
    , m_CommandPool(other.m_CommandPool)
{
}

CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept
{
    m_CommandBuffer = std::move(other.m_CommandBuffer);
    m_CommandPool = other.m_CommandPool;
    return *this;
}

void CommandBuffer::Begin(CommandBufferUsage commandBufferUsage)
{
    VkCommandBufferBeginInfo commandBufferBeginInfo {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VkCommandBufferUsageFlags usageFlags = static_cast<VkCommandBufferUsageFlags>(commandBufferUsage);
    commandBufferBeginInfo.flags = usageFlags;

    assert(vkBeginCommandBuffer(m_CommandBuffer, &commandBufferBeginInfo) == VK_SUCCESS);
}

void CommandBuffer::End()
{
    assert(vkEndCommandBuffer(m_CommandBuffer) == VK_SUCCESS);
}

void CommandBuffer::BeginRenderPass(VkRenderPassBeginInfo const& renderPass)
{
    vkCmdBeginRenderPass(m_CommandBuffer, &renderPass, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::EndRenderPass()
{
    vkCmdEndRenderPass(m_CommandBuffer);
}

void CommandBuffer::BindPipeline(VkPipeline const& pipeline)
{
    vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void CommandBuffer::BindVertexBuffer(GPUBuffer const* buffer)
{
    VkBuffer vertexBuffer = buffer->GetVulkanBuffer();
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(m_CommandBuffer, 0, 1, &vertexBuffer, offsets);
}

void CommandBuffer::BindIndexBuffer(GPUBuffer const* buffer)
{
    vkCmdBindIndexBuffer(m_CommandBuffer, buffer->GetVulkanBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void CommandBuffer::SetScissor(VkRect2D const& scissorState)
{
    vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissorState);
}

void CommandBuffer::SetViewport(VkViewport const& viewport)
{
    vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);
}

void CommandBuffer::BindDescriptorSets(VkPipelineLayout const& pipelineLayout, VkDescriptorSet const& descriptorSet)
{
    vkCmdBindDescriptorSets(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipelineLayout, 0, 1, &descriptorSet,
        0, nullptr);
}

void CommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
    int32_t vertexOffset, uint32_t firstInstance)
{
    vkCmdDrawIndexed(m_CommandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

VkCommandBuffer const& CommandBuffer::GetVulkanCommandBuffer() const
{
    return m_CommandBuffer;
}

void CommandBuffer::TransitionLayout(Image* image, VkImageLayout newLayout)
{
    if (newLayout == image->m_CurrentLayout)
        return;

    VkImageMemoryBarrier memoryBarrier {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.image = image->m_Image;
    memoryBarrier.oldLayout = image->m_CurrentLayout;
    memoryBarrier.newLayout = newLayout;
    memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.subresourceRange.baseArrayLayer = 0;
    memoryBarrier.subresourceRange.baseMipLevel = 0;
    memoryBarrier.subresourceRange.layerCount = 1;
    memoryBarrier.subresourceRange.levelCount = 1;
    memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    // VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    // VkCommandPool transientCommandPool = VK_NULL_HANDLE;
    // LogicalDevice* logicalDevice = RendererContext::GetLogicalDevice();

    // if (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    //     transientCommandPool =
    //     RendererContext::GetTransientGraphicsCommandPool();
    // else
    //     transientCommandPool =
    //     RendererContext::GetTransientTransferCommandPool();

    // assert(transientCommandPool != VK_NULL_HANDLE);

    // commandBuffer =
    //     RendererContext::AllocateCommandBuffer(transientCommandPool);
    // RendererContext::BeginCommandBuffer(
    //     commandBuffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkPipelineStageFlags srcStageFlags = 0;
    VkPipelineStageFlags dstStageFlags = 0;

    if (image->m_CurrentLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;

        memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    } else if (image->m_CurrentLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        srcStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

        memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    } else {
        assert(false);
    }

    vkCmdPipelineBarrier(m_CommandBuffer, srcStageFlags, dstStageFlags,
        0, 0, nullptr,
        0, nullptr, 1, &memoryBarrier);
    // RendererContext::EndCommandBuffer(m_CommandBuffer);

    /*VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_CommandBuffer;*/

    /*VkQueue transitionQueue;
    if (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        transitionQueue = logicalDevice->GetTransferQueue();
    else
        transitionQueue = logicalDevice->GetGraphicsQueue();*/

    // logicalDevice->QueueSubmit(transitionQueue, 1, &submitInfo, VK_NULL_HANDLE);
    // logicalDevice->QueueWaitIdle(transitionQueue);

    /*vkFreeCommandBuffers(logicalDevice->GetVulkanDevice(), transientCommandPool, 1, &commandBuffer);*/

    image->m_CurrentLayout = newLayout;
}

void CommandBuffer::CopyBufferToImage(GPUBuffer* source, Image* destination, uint32_t width, uint32_t height)
{
    VkBufferImageCopy bufferImageCopy {};
    bufferImageCopy.bufferOffset = 0;
    /* If either of these values is zero,
     * that aspect of the buffer memory is considered to be tightly packed
     * according to the imageExtent. */
    bufferImageCopy.bufferImageHeight = 0;
    bufferImageCopy.bufferRowLength = 0;

    // TODO: make image subresource user specifiable
    bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferImageCopy.imageSubresource.baseArrayLayer = 0;
    bufferImageCopy.imageSubresource.layerCount = 1;
    bufferImageCopy.imageSubresource.mipLevel = 0;

    // TODO: make image offset and image extent user specifiable
    bufferImageCopy.imageOffset = { 0, 0, 0 };
    bufferImageCopy.imageExtent = { width, height, 1 };

    vkCmdCopyBufferToImage(m_CommandBuffer, source->GetVulkanBuffer(),
        destination->GetVulkanImage(), destination->GetCurrentVulkanLayout(),
        1, &bufferImageCopy);
}

void CommandBuffer::CopyBuffer(GPUBuffer const* source, GPUBuffer const* destination, size_t size)
{
    VkBufferCopy bufferCopy;
    bufferCopy.dstOffset = 0;
    bufferCopy.size = size;
    bufferCopy.srcOffset = 0;
    vkCmdCopyBuffer(m_CommandBuffer, source->GetVulkanBuffer(),
        destination->GetVulkanBuffer(), 1, &bufferCopy);
}

/*void CommandBuffer::AllocateCommandBuffer(VkCommandPool commandPool,
    VkCommandBufferLevel commandBufferLevel)
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.
}*/

}
