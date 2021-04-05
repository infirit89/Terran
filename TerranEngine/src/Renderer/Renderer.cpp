#include "trpch.h"

#include "Renderer.h"

#include <glad/glad.h>
namespace TerranEngine 
{
	void Renderer::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer::Resize(int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void Renderer::Draw(const std::shared_ptr<VertexArray>& vertexArray, int numIndices)
	{
		vertexArray->Bind();

		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
	}
}

