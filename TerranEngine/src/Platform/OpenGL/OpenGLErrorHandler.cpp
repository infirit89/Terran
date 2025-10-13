#include "OpenGLErrorHandler.h"
#include "trpch.h"

#include "LibCore/Log.h"

namespace TerranEngine {

static spdlog::level::level_enum GetSPDLogLevel(uint32_t openglDebugSeverity)
{
    switch (openglDebugSeverity) {
    case GL_DEBUG_SEVERITY_HIGH:
        return spdlog::level::level_enum::err;
    case GL_DEBUG_SEVERITY_MEDIUM:
        return spdlog::level::level_enum::warn;
    case GL_DEBUG_SEVERITY_LOW:
        return spdlog::level::level_enum::trace;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        return spdlog::level::level_enum::info;
    default:;
    }

    return spdlog::level::level_enum::info;
}

void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, char const* message, void const* userParam)
{
    {
        // ignore non-significant error/warning codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
            return;

        spdlog::level::level_enum logLevel = GetSPDLogLevel(severity);

        Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "---------------");

        switch (source) {
        case GL_DEBUG_SOURCE_API:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Source: API");
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Source: Window System");
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Source: Shader Compiler");
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Source: Third Party");
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Source: Application");
            break;
        case GL_DEBUG_SOURCE_OTHER:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Source: Other");
            break;
        default:;
        }

        switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Error");
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Deprecated Behaviour");
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Undefined Behaviour");
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Portability");
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Performance");
            break;
        case GL_DEBUG_TYPE_MARKER:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Marker");
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Push Group");
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Pop Group");
            break;
        case GL_DEBUG_TYPE_OTHER:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Other");
            break;
        default:;
        }

        switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Severity: high");
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Severity: medium");
            break;
        case GL_DEBUG_SEVERITY_LOW:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Severity: low");
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Severity: notification");
            break;
        default:;
        }

        Terran::Core::Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Debug message (" + std::to_string(id) + "): " + message);
    }
}

}

