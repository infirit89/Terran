#pragma once
#include "vulkan/vulkan_core.h"

#include "GPUBuffer.h"
#include "Image.h"

namespace Terran::Graphics {

enum class CommandBufferUsage {
    None = 0,
    OneTimeSubmit = 1,
};

class CommandBuffer {
public:
    // NOTE: transfers ownership to this class
    CommandBuffer(VkCommandPool const& commandPool, VkCommandBuffer&& commandBuffer);

    // CommandBuffer(const VkCommandPool& commandPool, VkCommandBufferLevel commandBufferLevel);
    ~CommandBuffer();
    CommandBuffer(CommandBuffer const& other) = delete;
    CommandBuffer(CommandBuffer&& other) noexcept;

    CommandBuffer& operator=(CommandBuffer&& other) noexcept;
    CommandBuffer& operator=(CommandBuffer& other) = delete;

    void Begin(CommandBufferUsage commandBufferUsage = CommandBufferUsage::None);
    void End();
    void BeginRenderPass(VkRenderPassBeginInfo const& renderPass);
    void EndRenderPass();

    void BindPipeline(VkPipeline const& pipeline);
    // TODO: bind vertex buffers
    void BindVertexBuffer(GPUBuffer const* buffer);
    void BindIndexBuffer(GPUBuffer const* buffer);

    void SetScissor(VkRect2D const& scissorState);
    void SetViewport(VkViewport const& viewport);

    void BindDescriptorSets(VkPipelineLayout const& pipelineLayout, VkDescriptorSet const&);

    void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);

    VkCommandBuffer const& GetVulkanCommandBuffer() const;

    void TransitionLayout(Image* image, VkImageLayout newLayout);
    void CopyBufferToImage(GPUBuffer* source, Image* destination, uint32_t width, uint32_t height);
    void CopyBuffer(GPUBuffer const* source, GPUBuffer const* destination, size_t size);

private:
    // void AllocateCommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel commandBufferLevel);

private:
    VkCommandBuffer m_CommandBuffer;
    VkCommandPool m_CommandPool;

    friend class RendererContext;
};

}
