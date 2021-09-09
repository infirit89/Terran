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

		if (GLVersion.major >= 4 && GLVersion.minor >= 3)
		{
			int flags;
			glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

			if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
			{
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				glDebugMessageCallback(glDebugOutput, nullptr);
				glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
			}

		}

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

	void RenderCommand::EnableBlending() 
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		//glEnable(GL_DEPTH_TEST);
	}

	void RenderCommand::Draw(const VertexArray& vertexArray, int numIndices)
	{
		vertexArray.Bind();

		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
	}

	void RenderCommand::DrawArrays(int numIndices)
	{
		glDrawArrays(GL_TRIANGLES, 0, numIndices);
	}
}

