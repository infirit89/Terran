#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Terran::Graphics {

class Framebuffer {
public:
    Framebuffer(VkRenderPass renderPass, std::vector<VkImageView> const& attachment, uint32_t width, uint32_t height);
    ~Framebuffer();

    void Resize(std::vector<VkImageView> const& attachment, uint32_t width, uint32_t height);
    VkFramebuffer GetVulkanFramebuffer() const { return m_Framebuffer; }

private:
    void Create();
    void Destroy();

private:
    VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
    std::vector<VkImageView> m_Attachments;
    VkRenderPass m_RenderPass;
    uint32_t m_Width, m_Height;
};

}
