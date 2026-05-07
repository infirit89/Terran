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

class SamplerSpecification {
public:
    SamplerSpecification& set_magnification_filter(TextureFilter filter) {
        MagnificationFilter = filter;
        return *this;
    }

    SamplerSpecification& set_minification_filter(TextureFilter filter) {
        MinificationFilter = filter;
        return *this;
    }

    SamplerSpecification& set_mipmap_filter(TextureFilter filter) {
        MipmapFilter = filter;
        return *this;
    }

    SamplerSpecification& set_address_mode_u(TextureAddressMode addres_mode) {
        AddressModeU = addres_mode;
        return *this;
    }

    SamplerSpecification& set_address_mode_v(TextureAddressMode addres_mode) {
        AddressModeV = addres_mode;
        return *this;
    }

    SamplerSpecification& set_address_mode_w(TextureAddressMode addres_mode) {
        AddressModeW = addres_mode;
        return *this;
    }

    SamplerSpecification& set_anisotropy(bool enable_anisotropy) {
        UseAnisotropy = enable_anisotropy;
        return *this;
    }

    TextureFilter MagnificationFilter;
    TextureFilter MinificationFilter;
    TextureFilter MipmapFilter;

    TextureAddressMode AddressModeU;
    TextureAddressMode AddressModeV;
    TextureAddressMode AddressModeW;
    bool UseAnisotropy;
};

class Sampler {
public:
    Sampler(SamplerSpecification const& createInfo);
    ~Sampler();

    Sampler(Sampler const& other) = delete;
    Sampler(Sampler&& other) = delete;
    Sampler& operator=(Sampler const& other) = delete;

    VkSampler const& GetVulkanSampler() const { return m_Sampler; }

private:
    void Create(SamplerSpecification const& createInfo);

private:
    VkSampler m_Sampler;
};

}
