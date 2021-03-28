#include "trpch.h"

#include "RendererCommand.h"

#include <glad/glad.h>
namespace TerranEngine 
{
	void RendererCommand::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void RendererCommand::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void RendererCommand::Resize(int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void RendererCommand::Draw(const std::shared_ptr<VertexArray>& vertexArray, int numIndices)
	{
		vertexArray->Bind();

		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
	}
}

