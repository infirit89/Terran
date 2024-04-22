#include "trpch.h"
#include "OpenGLErrorHandler.h"

#include "Core/Log.h"

namespace TerranEngine 
{
	static spdlog::level::level_enum GetSPDLogLevel(uint32_t openglDebugSeverity) 
	{
		switch (openglDebugSeverity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         return spdlog::level::level_enum::err;
		case GL_DEBUG_SEVERITY_MEDIUM:       return spdlog::level::level_enum::warn;
		case GL_DEBUG_SEVERITY_LOW:          return spdlog::level::level_enum::trace;
		case GL_DEBUG_SEVERITY_NOTIFICATION: return spdlog::level::level_enum::info;
		}

		return spdlog::level::level_enum::info;
	}

	void glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
	{
		{
			// ignore non-significant error/warning codes
			if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

			spdlog::level::level_enum logLevel = GetSPDLogLevel(severity);

			Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "---------------");

			switch (source)
			{
			case GL_DEBUG_SOURCE_API:				Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Source: API"); break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Source: Window System"); break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER:	Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Source: Shader Compiler"); break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:		Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Source: Third Party"); break;
			case GL_DEBUG_SOURCE_APPLICATION:		Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Source: Application"); break;
			case GL_DEBUG_SOURCE_OTHER:				Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Source: Other"); break;
			}

			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR:               Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Error"); break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Deprecated Behaviour"); break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Undefined Behaviour"); break;
			case GL_DEBUG_TYPE_PORTABILITY:         Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Portability"); break;
			case GL_DEBUG_TYPE_PERFORMANCE:         Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Performance"); break;
			case GL_DEBUG_TYPE_MARKER:              Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Marker"); break;
			case GL_DEBUG_TYPE_PUSH_GROUP:          Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Push Group"); break;
			case GL_DEBUG_TYPE_POP_GROUP:           Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Pop Group"); break;
			case GL_DEBUG_TYPE_OTHER:               Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Type: Other"); break;
			}

			switch (severity)
			{
			case GL_DEBUG_SEVERITY_HIGH:			Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Severity: high"); break;
			case GL_DEBUG_SEVERITY_MEDIUM:			Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Severity: medium"); break;
			case GL_DEBUG_SEVERITY_LOW:				Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Severity: low"); break;
			case GL_DEBUG_SEVERITY_NOTIFICATION:	Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Severity: notification"); break;
			}

			Log::GetLogger(TR_LOG_RENDERER)->log(logLevel, "Debug message (" + std::to_string(id) + "): " + message);
		}
	}
}