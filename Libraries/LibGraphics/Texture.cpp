#include "Texture.h"
#include "GPUBuffer.h"
#include "Image.h"
#include "LogicalDevice.h"
#include "RendererContext.h"
#include "CommandBuffer.h"
#include "Sampler.h"

#include <cstring>

namespace Terran::Graphics {

Texture::Texture(LogicalDevice* logical_device,
    TextureSpecification const& specification, void* texture_data)
    : m_texture_size(specification.Width * specification.Height * specification.Channels)
    , m_logical_device(logical_device)
{
    GPUBuffer stagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_texture_size,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data = stagingBuffer.MapMemory();
    std::memcpy(data, texture_data, m_texture_size);
    stagingBuffer.UnmapMemory();

    ImageCreateInfo imageCreateInfo;
    imageCreateInfo.Width = specification.Width;
    imageCreateInfo.Height = specification.Height;
    imageCreateInfo.Format = VK_FORMAT_R8G8B8A8_SRGB;
    imageCreateInfo.Tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.Usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageCreateInfo.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    imageCreateInfo.AspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

    m_image = new Image(imageCreateInfo);
    LogicalDevice* logicalDevice = m_logical_device;

    CommandBuffer transferCommandBuffer = RendererContext::CreateStackCommandBuffer(RendererContext::GetTransientTransferCommandPool());
    transferCommandBuffer.Begin(CommandBufferUsage::OneTimeSubmit);
    transferCommandBuffer.TransitionLayout(m_image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    transferCommandBuffer.CopyBufferToImage(
        &stagingBuffer, m_image, specification.Width, specification.Height);
    transferCommandBuffer.End();

    logicalDevice->SubmitImmediateCommands(transferCommandBuffer,
        logicalDevice->GetTransferQueue());

    CommandBuffer graphicsCommandBuffer = RendererContext::CreateStackCommandBuffer(RendererContext::GetTransientGraphicsCommandPool());
    graphicsCommandBuffer.Begin(CommandBufferUsage::OneTimeSubmit);
    graphicsCommandBuffer.TransitionLayout(
        m_image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    graphicsCommandBuffer.End();

    logicalDevice->SubmitImmediateCommands(graphicsCommandBuffer,
        logicalDevice->GetGraphicsQueue());

    SamplerCreateInfo samplerCreateInfo {
        .MagFilter = specification.MagnificationFilter,
        .MinFilter = specification.MinificationFilter,
        .MipmapFilter = specification.MipmapFilter,
        .AddressModeU = specification.AddressModeU,
        .AddressModeV = specification.AddressModeV,
        .AnisotropyEnable = specification.UseAnisotropy,
    };

    m_sampler = new Sampler(samplerCreateInfo);
}
Texture::~Texture()
{
    delete m_sampler;
    delete m_image;
}

} // namespace LearningVulkan
