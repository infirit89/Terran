#include "trpch.h"
#include "OpenGLRendererAPI.h"
#include "OpenGLErrorHandler.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace TerranEngine 
{
    OpenGLRendererAPI::OpenGLRendererAPI()
    {
        int success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        TR_ASSERT(success, "Couldn't initialize GLAD");

        TR_TRACE("Graphics card: {0}", glGetString(GL_RENDERER));
        TR_TRACE("OpenGL version: {0}", glGetString(GL_VERSION));

        TR_ASSERT(GetAPIVersion() >= TR_MINIMUM_OGL_VERSION, "Terran doesn't support opengl versions older than {0}", TR_MINIMUM_OGL_VERSION);

#ifdef TR_DEBUG
        if (GLVersion.major >= 4 && GLVersion.minor >= 3)
        {

            int flags;
            glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

            if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
            {
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                glDebugMessageCallback(glDebugOutput, nullptr);
                glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_TRUE);
            }
        }
#endif
    }

    void OpenGLRendererAPI::SetClearColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }

    void OpenGLRendererAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRendererAPI::Resize(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void OpenGLRendererAPI::ToggleWireframeMode(bool state)
    {
        // TODO: polygon mode?
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL - (int)state);
    }

    void OpenGLRendererAPI::ToggleBlending(bool state)
    {
        if (state)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
        else
            glDisable(GL_BLEND);
    }

    void OpenGLRendererAPI::ToggleDepthTesting(bool state)
    {
        if (state)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
        }
        else
            glDisable(GL_DEPTH_TEST);
    }

    static uint32_t ConvertRenderModeToNativeMode(RenderMode mode)
    {
        switch (mode)
        {
        case RenderMode::Points:
        case RenderMode::Lines:
        case RenderMode::LineLoop:
        case RenderMode::LineStrip:
        case RenderMode::Triangles:
        case RenderMode::TriangleStrip:
        case RenderMode::TriangleFan:
            return GL_POINTS + (uint32_t)mode;
        }

        TR_ERROR("Unsupported render mode");
        return GL_TRIANGLES;
    }


    void OpenGLRendererAPI::Draw(RenderMode mode, const Shared<VertexArray>& vertexArray, int indexCount)
    {
        uint32_t nativeMode = ConvertRenderModeToNativeMode(mode);
        glDrawElements(nativeMode, indexCount, GL_UNSIGNED_INT, nullptr);
    }

    void OpenGLRendererAPI::DrawArrays(RenderMode mode, int vertexCount)
    {
        uint32_t nativeMode = ConvertRenderModeToNativeMode(mode);
        glDrawArrays(nativeMode, 0, vertexCount);
    }

    void OpenGLRendererAPI::SetLineWidth(float lineWidth)
    {
        glLineWidth(lineWidth);
    }

    uint32_t OpenGLRendererAPI::GetAPIVersion()
    {
        return (GLVersion.major * 100) + (GLVersion.minor * 10);
    }
}

