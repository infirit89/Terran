#include "trpch.h"
#include "OpenGLErrorHandler.h"

namespace TerranEngine 
{
	void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
	{
        {
            // ignore non-significant error/warning codes
            if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

            TR_WARN("---------------");

            switch (source)
            {
            case GL_DEBUG_SOURCE_API:             TR_WARN("Source: API"); break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   TR_WARN("Source: Window System"); break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: TR_WARN("Source: Shader Compiler"); break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:     TR_WARN("Source: Third Party"); break;
            case GL_DEBUG_SOURCE_APPLICATION:     TR_WARN("Source: Application"); break;
            case GL_DEBUG_SOURCE_OTHER:           TR_WARN("Source: Other"); break;
            }

            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR:               TR_WARN("Type: Error"); break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: TR_WARN("Type: Deprecated Behaviour"); break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  TR_WARN("Type: Undefined Behaviour"); break;
            case GL_DEBUG_TYPE_PORTABILITY:         TR_WARN("Type: Portability"); break;
            case GL_DEBUG_TYPE_PERFORMANCE:         TR_WARN("Type: Performance"); break;
            case GL_DEBUG_TYPE_MARKER:              TR_WARN("Type: Marker"); break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          TR_WARN("Type: Push Group"); break;
            case GL_DEBUG_TYPE_POP_GROUP:           TR_WARN("Type: Pop Group"); break;
            case GL_DEBUG_TYPE_OTHER:               TR_WARN("Type: Other"); break;
            }

            switch (severity)
            {
            case GL_DEBUG_SEVERITY_HIGH:         TR_WARN("Severity: high"); break;
            case GL_DEBUG_SEVERITY_MEDIUM:       TR_WARN("Severity: medium"); break;
            case GL_DEBUG_SEVERITY_LOW:          TR_WARN("Severity: low"); break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: TR_WARN("Severity: notification"); break;
            }

            TR_WARN("Debug message (" + std::to_string(id) + "): " + message);
        }
	}
}