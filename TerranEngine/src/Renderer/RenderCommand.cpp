#include "trpch.h"

#include "RenderCommand.h"

#include <glad/glad.h>
namespace TerranEngine 
{
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

	void RenderCommand::EnableBlending() 
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void RenderCommand::Draw(const VertexArray& vertexArray, int numIndices)
	{
		vertexArray.Bind();

		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
	}
}

