#include "trpch.h"

#include "OpenGLErrorHandler.h"

namespace TerranEngine 
{
	void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
	{
        {
            // ignore non-significant error/warning codes
            if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

            TR_TRACE("---------------");
            TR_TRACE("Debug message (" + std::to_string(id) + "): " + message);

            switch (source)
            {
            case GL_DEBUG_SOURCE_API:             TR_TRACE("Source: API"); break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   TR_TRACE("Source: Window System"); break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: TR_TRACE("Source: Shader Compiler"); break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:     TR_TRACE("Source: Third Party"); break;
            case GL_DEBUG_SOURCE_APPLICATION:     TR_TRACE("Source: Application"); break;
            case GL_DEBUG_SOURCE_OTHER:           TR_TRACE("Source: Other"); break;
            }

            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR:               TR_TRACE("Type: Error"); break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: TR_TRACE("Type: Deprecated Behaviour"); break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  TR_TRACE("Type: Undefined Behaviour"); break;
            case GL_DEBUG_TYPE_PORTABILITY:         TR_TRACE("Type: Portability"); break;
            case GL_DEBUG_TYPE_PERFORMANCE:         TR_TRACE("Type: Performance"); break;
            case GL_DEBUG_TYPE_MARKER:              TR_TRACE("Type: Marker"); break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          TR_TRACE("Type: Push Group"); break;
            case GL_DEBUG_TYPE_POP_GROUP:           TR_TRACE("Type: Pop Group"); break;
            case GL_DEBUG_TYPE_OTHER:               TR_TRACE("Type: Other"); break;
            }

            switch (severity)
            {
            case GL_DEBUG_SEVERITY_HIGH:         TR_TRACE("Severity: high"); break;
            case GL_DEBUG_SEVERITY_MEDIUM:       TR_TRACE("Severity: medium"); break;
            case GL_DEBUG_SEVERITY_LOW:          TR_TRACE("Severity: low"); break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: TR_TRACE("Severity: notification"); break;
            }
        }
	}
}