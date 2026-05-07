#include "Sampler.h"

#include "LogicalDevice.h"
#include "RendererContext.h"

namespace Terran::Graphics {
namespace {

}

Sampler::Sampler(SamplerSpecification const& createInfo)
{
    Create(createInfo);
}

Sampler::~Sampler()
{
    LogicalDevice* logicalDevice = RendererContext::GetLogicalDevice();
    vkDestroySampler(logicalDevice->GetVulkanDevice(), m_Sampler, nullptr);
}

void Sampler::Create(SamplerSpecification const& createInfo)
{
    VkSamplerCreateInfo samplerCreateInfo {};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = static_cast<VkFilter>(createInfo.MagnificationFilter);
    samplerCreateInfo.minFilter = static_cast<VkFilter>(createInfo.MinificationFilter);
    samplerCreateInfo.mipmapMode = static_cast<VkSamplerMipmapMode>(createInfo.MipmapFilter);
    samplerCreateInfo.addressModeU = static_cast<VkSamplerAddressMode>(createInfo.AddressModeU);
    samplerCreateInfo.addressModeV = static_cast<VkSamplerAddressMode>(createInfo.AddressModeV);
    samplerCreateInfo.anisotropyEnable = createInfo.UseAnisotropy;

    LogicalDevice* logicalDevice = RendererContext::GetLogicalDevice();
    PhysicalDevice* physicalDevice = logicalDevice->GetPhysicalDevice();

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice->GetPhysicalDevice(), &physicalDeviceProperties);
    samplerCreateInfo.maxAnisotropy = physicalDeviceProperties.limits.maxSamplerAnisotropy;

    assert(vkCreateSampler(logicalDevice->GetVulkanDevice(),
               &samplerCreateInfo, nullptr, &m_Sampler)
        == VK_SUCCESS);
}

}
