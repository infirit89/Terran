#include "Texture.h"
#include "GPUBuffer.h"
#include "Image.h"
#include "LogicalDevice.h"
#include "RendererContext.h"

#include <cstring>

namespace Terran::Graphics {

Texture::Texture(LogicalDevice* logical_device, void* texture_data, int width,
    int height, int channels)
    : m_texture_size(width * height * 4)
    , m_logical_device(logical_device)
{
    GPUBuffer stagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, m_texture_size,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data = stagingBuffer.MapMemory();
    std::memcpy(data, texture_data, m_texture_size);
    stagingBuffer.UnmapMemory();

    ImageCreateInfo imageCreateInfo;
    imageCreateInfo.Width = width;
    imageCreateInfo.Height = height;
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
    transferCommandBuffer.CopyBufferToImage(&stagingBuffer, m_image, width,
        height);
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
        .MagFilter = TextureFilter::Nearest,
        .MinFilter = TextureFilter::Nearest,
        .MipmapFilter = TextureFilter::Nearest,
        .AddressModeU = TextureAddressMode::Repeat,
        .AddressModeV = TextureAddressMode::Repeat,
        .AnisotropyEnable = true,
    };

    m_sampler = new Sampler(samplerCreateInfo);
}
Texture::~Texture()
{
    delete m_sampler;
    delete m_image;
}

} // namespace LearningVulkan
