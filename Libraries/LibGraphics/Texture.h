#pragma once

#include "Image.h"
#include "Sampler.h"

#include <cstddef>
#include <filesystem>

namespace Terran::Graphics {

class LogicalDevice;
class TextureSpecification {
public:
    TextureSpecification& set_width(size_t width)
    {
        Width = width;
        return *this;
    }

    TextureSpecification& set_height(size_t height)
    {
        Height = height;
        return *this;
    }

    TextureSpecification& set_channels(size_t channels)
    {
        Channels = channels;
        return *this;
    }

    TextureSpecification& set_size(size_t width, size_t height)
    {
        Width = width;
        Height = height;
        return *this;
    }

    TextureSpecification& set_magnification_filter(TextureFilter filter)
    {
        MagnificationFilter = filter;
        return *this;
    }

    TextureSpecification& set_minification_filter(TextureFilter filter)
    {
        MinificationFilter = filter;
        return *this;
    }

    TextureSpecification& set_mipmap_filter(TextureFilter filter)
    {
        MipmapFilter = filter;
        return *this;
    }

    TextureSpecification& set_address_mode_u(TextureAddressMode address_mode)
    {
        AddressModeU = address_mode;
        return *this;
    }

    TextureSpecification& set_address_mode_v(TextureAddressMode address_mode)
    {
        AddressModeV = address_mode;
        return *this;
    }
    size_t Width;
    size_t Height;
    size_t Channels;
    TextureFilter MagnificationFilter = TextureFilter::Nearest;
    TextureFilter MinificationFilter = TextureFilter::Nearest;
    TextureFilter MipmapFilter = TextureFilter::Nearest;
    TextureAddressMode AddressModeU = TextureAddressMode::Repeat;
    TextureAddressMode AddressModeV = TextureAddressMode::Repeat;
    bool UseAnisotropy = true;
};

class Texture {

public:
    ~Texture();

    Image const* GetImage() const { return m_image; }

    Sampler const* GetSampler() const { return m_sampler; }

private:
    Texture(LogicalDevice* logical_device,
        TextureSpecification const& specification,
        void* data);

private:
    LogicalDevice* m_logical_device;
    Image* m_image;
    Sampler* m_sampler;
    size_t m_texture_size;

    friend class LogicalDevice;
};

}
