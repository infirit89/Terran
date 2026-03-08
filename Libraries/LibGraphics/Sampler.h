#pragma once
#include <vulkan/vulkan_core.h>

namespace Terran::Graphics {

enum class TextureFilter {
    Nearest = 0,
    Linear = 1,
};

enum class TextureAddressMode {
    Repeat = 0,
    MirroredRepeat = 1,
    ClampToEdge = 2,
    ClampToBorder = 3,
    MirroredClampToEdge = 4,
};

struct SamplerCreateInfo {
    TextureFilter MagFilter;
    TextureFilter MinFilter;
    TextureFilter MipmapFilter;

    TextureAddressMode AddressModeU;
    TextureAddressMode AddressModeV;
    TextureAddressMode AddressModeW;
    bool AnisotropyEnable;
};

class Sampler {
public:
    Sampler(SamplerCreateInfo const& createInfo);
    ~Sampler();

    Sampler(Sampler const& other) = delete;
    Sampler(Sampler&& other) = delete;
    Sampler& operator=(Sampler const& other) = delete;

    VkSampler const& GetVulkanSampler() const { return m_Sampler; }

private:
    void Create(SamplerCreateInfo const& createInfo);

private:
    VkSampler m_Sampler;
};

}
