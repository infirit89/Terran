#pragma once

#include "GPUBuffer.h"
#include "Image.h"
#include "Sampler.h"

namespace Terran::Graphics {

class LogicalDevice;
class Texture {

public:
    Texture(LogicalDevice* logical_device, void* texture_data, int width, int height, int channels);
    ~Texture();

    Image const* GetImage() const { return m_image; }

    Sampler const* GetSampler() const { return m_sampler; }

private:
    LogicalDevice* m_logical_device;
    Image* m_image;
    Sampler* m_sampler;
    size_t m_texture_size;
};

} // namespace LearningVulkan
