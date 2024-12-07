#include "trpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLErrorHandler.h"
#include "ShaderLibrary.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace TerranEngine 
{
	RendererData* Renderer::s_RendererData;

	void Renderer::Initialize()
	{
		int gladSuccess = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		TR_ASSERT(gladSuccess, "Couldn't initialize GLAD");

		TR_CORE_TRACE(TR_LOG_RENDERER, "Graphics card: {0}", glGetString(GL_RENDERER));
		TR_CORE_TRACE(TR_LOG_RENDERER, "OpenGL version: {0}", glGetString(GL_VERSION));

		TR_ASSERT(GLVersion.major >= 4 && GLVersion.minor > 5, "Terran doesn't support opengl versions older than 4.5.0");

#ifdef TR_DEBUG
		int flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_TRUE);
		}
		TR_CORE_INFO(TR_LOG_RENDERER, "Setup opengl debug layer");
#endif
		s_RendererData = new RendererData;
		s_RendererData->CreateResourceQueue = CreateShared<CommandQueue>();
		s_RendererData->RenderQueue = CreateShared<CommandQueue>();
		s_RendererData->FreeResourceQueue = CreateShared<CommandQueue>();

		EnableBlending(true);
		EnableDepthTesting(true);

		ShaderLibrary::Initialize();
		TR_CORE_INFO(TR_LOG_RENDERER, "Initialized renderer");
	}

	void Renderer::Shutdown()
	{
		s_RendererData->CreateResourceQueue = nullptr;
		s_RendererData->RenderQueue = nullptr;
		s_RendererData->FreeResourceQueue = nullptr;
		delete s_RendererData;
		TR_CORE_INFO(TR_LOG_RENDERER, "Destroyed renderer");
	}

	void Renderer::ExecuteCommands()
	{
		s_RendererData->CreateResourceQueue->Execute();
		s_RendererData->RenderQueue->Execute();
		s_RendererData->FreeResourceQueue->Execute();
	}

	void Renderer::SetClearColor(float r, float g, float b, float a) 
	{
		Renderer::Submit([r, g, b, a]()
		{
			glClearColor(r, g, b, a);
		});
	}

	void Renderer::Clear() 
	{
		Renderer::Submit([]()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		});
	}

	void Renderer::SetViewport(int width, int height) 
	{
		Renderer::Submit([width, height]()
		{
			glViewport(0, 0, width, height);
		});
	}

	void Renderer::WireframeMode(bool enable)
	{
		Renderer::Submit([enable]() 
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
		});
	}

	void Renderer::EnableBlending(bool state)
	{
		Renderer::Submit([state]()
		{
			if (state) 
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			} else
				glDisable(GL_BLEND);
		});
	}

	void Renderer::EnableDepthTesting(bool state)
	{
		Renderer::Submit([state]()
		{
			if (state) 
			{
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
			}
			else
				glDisable(GL_DEPTH_TEST);
		});
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
			return GL_POINTS + static_cast<uint32_t>(mode);
		default:
			TR_CORE_ERROR(TR_LOG_RENDERER, "Unsupported render mode");
			break;
		}

		return GL_TRIANGLES;
	}

	void Renderer::DrawIndexed(RenderMode mode, const Shared<VertexArray>& vertexArray, int numIndices)
	{
		vertexArray->Bind();

		Renderer::Submit([mode, numIndices]()
		{
			uint32_t nativeMode = ConvertRenderModeToNativeMode(mode);
			glDrawElements(nativeMode, numIndices, GL_UNSIGNED_INT, nullptr);
		});
	}

	void Renderer::DrawArrays(RenderMode mode, const Shared<VertexArray>& vertexArray, int numVertices)
	{
		vertexArray->Bind();

		Renderer::Submit([mode, numVertices]() 
		{
			uint32_t nativeMode = ConvertRenderModeToNativeMode(mode);
			glDrawArrays(nativeMode, 0, numVertices);
		});
	}

	void Renderer::DrawInstanced(RenderMode mode, const Shared<VertexArray>& vertexArray, int instanceCount)
	{
		vertexArray->Bind();
		Renderer::Submit([mode, instanceCount]() 
		{
			uint32_t nativeMode = ConvertRenderModeToNativeMode(mode);
			glDrawElementsInstanced(nativeMode, 6, GL_UNSIGNED_SHORT, nullptr, instanceCount);
		});
	} 

	void Renderer::SetLineWidth(float lineWidth) 
	{
		Renderer::Submit([lineWidth]() 
		{
			glLineWidth(lineWidth);
		});
	}

	uint32_t Renderer::GetAPIVersion()
	{
		return (GLVersion.major * 100) + (GLVersion.minor * 10);
	}
}

