#pragma once

#include "LibCore/Base.h"
#include "Texture.h"

#include <stdint.h>
#include <vector>

namespace TerranEngine {

struct FramebufferParameters final {
    uint32_t Width = 1080, Height = 790;
    std::vector<TextureFormat> ColorAttachments;
    TextureFormat DepthAttachment = TextureFormat::Depth24Stencil8;
    uint32_t Samples = 1;
};

class Framebuffer final {
public:
    Framebuffer(FramebufferParameters const& parameters);
    ~Framebuffer();

    void Bind() const;
    void Unbind() const;

    void Resize(uint32_t width, uint32_t height);
    int ReadPixel_RT(uint32_t colorAttachmentIndex, int x, int y);

    Terran::Core::Shared<Texture2D> GetColorAttachment(uint32_t colorAttachmentIndex)
    {
        if (m_ColorAttachments.empty())
            return nullptr;

        return m_ColorAttachments[colorAttachmentIndex];
    }
    Terran::Core::Shared<Texture2D> GetDepthAttachment() { return m_DepthAttachment; }

    uint32_t GetWidth() const { return m_Parameters.Width; }
    uint32_t GetHeight() const { return m_Parameters.Height; }

    void SetColorAttachmentValue(uint32_t colorAttachmentIndex, int value);

private:
    void Create();
    void Release();
    void Release_RT();

private:
    uint32_t m_Handle;
    Terran::Core::Shared<Texture2D> m_DepthAttachment;
    std::vector<Terran::Core::Shared<Texture2D>> m_ColorAttachments;
    FramebufferParameters m_Parameters;
};

}
