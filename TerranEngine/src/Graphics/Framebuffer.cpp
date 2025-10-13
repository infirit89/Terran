#include "trpch.h"

#include "Framebuffer.h"
#include "Renderer.h"

#include <glad/glad.h>

namespace TerranEngine {

static constexpr size_t s_MaxSupportedColorAttachments = 4;
static GLenum s_Buffers[s_MaxSupportedColorAttachments] = {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3
};

Framebuffer::Framebuffer(FramebufferParameters const& parameters)
    : m_Handle(0)
    , m_Parameters(parameters)
{
    Create();
}

Framebuffer::~Framebuffer()
{
    Release();
}

void Framebuffer::Release()
{
    Renderer::SubmitFree([this]() {
        Release_RT();
    });
}

void Framebuffer::Release_RT()
{
    glDeleteFramebuffers(1, &m_Handle);
    m_Handle = 0;
}

void Framebuffer::Bind() const
{
    Renderer::Submit([this]() {
        TR_ASSERT(m_Handle != 0, "Handle is 0");
        if (m_Handle == 0)
            TR_ASSERT(false, "");

        glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
        glViewport(0, 0, m_Parameters.Width, m_Parameters.Height);
    });
}

void Framebuffer::Unbind() const
{
    Renderer::Submit([]() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });
}

void Framebuffer::Resize(uint32_t width, uint32_t height)
{
    m_Parameters.Width = width;
    m_Parameters.Height = height;

    Create();
}

int Framebuffer::ReadPixel_RT(uint32_t colorAttachmentIndex, int x, int y)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachmentIndex);
    int pixel;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);
    return pixel;
}

void Framebuffer::SetColorAttachmentValue(uint32_t colorAttachmentIndex, int value)
{
    Renderer::Submit([this, colorAttachmentIndex, value]() {
        int val = value;
        glClearTexImage(m_ColorAttachments[colorAttachmentIndex]->GetHandle(), 0,
            GL_RED_INTEGER, GL_INT, &val);
    });
}

void Framebuffer::Create()
{
    if (m_Handle) {
        m_DepthAttachment->Release();
        m_DepthAttachment = nullptr;

        for (auto const& colorAttachment : m_ColorAttachments)
            colorAttachment->Release();
        m_ColorAttachments.clear();
    }

    m_ColorAttachments.resize(m_Parameters.ColorAttachments.size());

    for (int i = 0; i < m_ColorAttachments.size(); i++) {
        TextureParameters attachmentParameters;
        attachmentParameters.Format = m_Parameters.ColorAttachments[i];
        attachmentParameters.Width = m_Parameters.Width;
        attachmentParameters.Height = m_Parameters.Height;
        attachmentParameters.Samples = m_Parameters.Samples;

        Terran::Core::Shared<Texture2D> attachment = Terran::Core::CreateShared<Texture2D>(attachmentParameters);

        m_ColorAttachments[i] = attachment;
    }

    TextureParameters attachmentParameters;
    attachmentParameters.Format = m_Parameters.DepthAttachment;
    attachmentParameters.Width = m_Parameters.Width;
    attachmentParameters.Height = m_Parameters.Height;
    attachmentParameters.Samples = m_Parameters.Samples;
    m_DepthAttachment = Terran::Core::CreateShared<Texture2D>(attachmentParameters);

    Renderer::SubmitCreate([colorAttachments = m_ColorAttachments, depthAttachment = m_DepthAttachment, this]() {
        if (m_Handle)
            Release_RT();

        glCreateFramebuffers(1, &m_Handle);

        for (int i = 0; i < colorAttachments.size(); i++) {
            glNamedFramebufferTexture(m_Handle,
                GL_COLOR_ATTACHMENT0 + i,
                colorAttachments[i]->GetHandle(), 0);
        }

        glNamedFramebufferTexture(m_Handle, GL_DEPTH_ATTACHMENT,
            depthAttachment->GetHandle(), 0);

        if (colorAttachments.size() > 1)
            glNamedFramebufferDrawBuffers(m_Handle, static_cast<int>(colorAttachments.size()), s_Buffers);

        if (glCheckNamedFramebufferStatus(m_Handle, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            TR_ASSERT(false, "Framebuffer isn't complete");
    });
}

}
