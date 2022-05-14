#include "trpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLErrorHandler.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace TerranEngine 
{
	void RenderCommand::Init()
	{
		int gladSuccess = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		TR_ASSERT(gladSuccess, "Couldn't initialize GLAD");

		TR_TRACE("Graphics card: {0}", glGetString(GL_RENDERER));
		TR_TRACE("OpenGL version: {0}", glGetString(GL_VERSION));

		TR_ASSERT(GLVersion.major > 3 || (GLVersion.major >= 3 && GLVersion.minor >= 2), "Terran doesn't support opengl versions older than 3.2.0");

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
		EnableBlending(true);
		EnableDepthTesting(true);
	}

	void RenderCommand::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void RenderCommand::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RenderCommand::Resize(int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void RenderCommand::WireframeMode(bool enable)
	{
		if (enable) 
		{
			glPolygonMode(GL_FRONT, GL_LINE);
			glPolygonMode(GL_BACK, GL_LINE);
		}
		else 
		{
			glPolygonMode(GL_FRONT, GL_FILL);
			glPolygonMode(GL_BACK, GL_FILL);
		}
	}

	void RenderCommand::EnableBlending(bool state) 
	{
		if (state) 
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		} else
			glDisable(GL_BLEND);
	}

	void RenderCommand::EnableDepthTesting(bool state)
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
		case TerranEngine::RenderMode::Points:
		case TerranEngine::RenderMode::Lines:
		case TerranEngine::RenderMode::LineLoop:
		case TerranEngine::RenderMode::LineStrip:
		case TerranEngine::RenderMode::Triangles:
		case TerranEngine::RenderMode::TriangleStrip:
		case TerranEngine::RenderMode::TriangleFan:
			return GL_POINTS + (uint32_t)mode;
		default:
			TR_ERROR("Unsupported render mode");
			break;
		}

		return GL_TRIANGLES;
	}

	void RenderCommand::Draw(RenderMode mode, const Shared<VertexArray>& vertexArray, int numIndices)
	{
		uint32_t nativeMode = ConvertRenderModeToNativeMode(mode);

		vertexArray->GetIndexBuffer()->Bind();

		glDrawElements(nativeMode, numIndices, GL_UNSIGNED_INT, nullptr);
	}

	void RenderCommand::DrawArrays(int numIndices)
	{
		glDrawArrays(GL_TRIANGLES, 0, numIndices);
	}

	uint32_t RenderCommand::GetAPIVersion()
	{
		return (GLVersion.major * 100) + (GLVersion.minor * 10);
	}
}

