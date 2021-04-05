#include "trpch.h"
#include "BatchRenderer.h"

#include <glm/vec3.hpp>

#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/Renderer.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	struct RendererData
	{
		uint32_t MaxVertices, MaxIndices;

		std::shared_ptr<VertexBuffer> VertexBuffer;
		std::shared_ptr<IndexBuffer> IndexBuffer;
		std::shared_ptr<VertexArray> VertexArray;
		std::shared_ptr<Shader> Shader;

		Vertex* VertexArrayPtr;
		uint32_t VertexIndex = 0;

		uint32_t IndexCount = 0;

		glm::vec4 VertexPositions[4];
	};

	static RendererData s_RData;

	void BatchRenderer::Init(uint32_t batchSize)
	{
		s_RData.MaxVertices = batchSize * 4;
		s_RData.MaxIndices = batchSize * 6;

		s_RData.VertexArrayPtr = new Vertex[s_RData.MaxVertices];

		s_RData.VertexArray = std::make_shared<VertexArray>();
		s_RData.VertexBuffer.reset(new VertexBuffer(s_RData.MaxVertices * sizeof(Vertex)));

		VertexBufferLayout layout = VertexBufferLayout({
			{ GL_FLOAT, 3 },
			{ GL_FLOAT, 4 }
		});

		s_RData.VertexArray->AddVertexBuffer(s_RData.VertexBuffer);
		s_RData.VertexArray->AddVertexBufferLayout(layout);

		int* indices = new int[s_RData.MaxIndices];

		int offset = 0;
		for (uint32_t i = 0; i < s_RData.MaxIndices; i += 6)
		{
			indices[i] = offset;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset;

			offset += 4;
		}

		s_RData.IndexBuffer.reset(new IndexBuffer(indices, s_RData.MaxIndices * sizeof(int)));

		delete[] indices;

		s_RData.Shader.reset(new Shader("res/VertexShader.glsl", "res/FragmentShader.glsl"));
		s_RData.Shader->Bind();

		s_RData.VertexPositions[0] = {	-0.5f, -0.5f, 0.0f, 1.0f };
		s_RData.VertexPositions[1] = {	 0.5f, -0.5f, 0.0f, 1.0f };
		s_RData.VertexPositions[2] = {	 0.5f,  0.5f, 0.0f, 1.0f };
		s_RData.VertexPositions[3] = {	-0.5f,  0.5f, 0.0f, 1.0f };
	}

	void BatchRenderer::Close()
	{
		delete[] s_RData.VertexArrayPtr;
	}

	void BatchRenderer::BeginScene(const Camera& camera, glm::mat4& cameraTransform)
	{
		s_RData.Shader->Bind();
		s_RData.Shader->UploadMat4("u_ProjMat", camera.ProjectionMatrix);
		s_RData.Shader->UploadMat4("u_ViewMat", cameraTransform);

		s_RData.IndexCount = 0;
		s_RData.VertexIndex = 0;
	}

	void BatchRenderer::Draw(glm::mat4& transform, glm::vec4 color)
	{
		if (s_RData.IndexCount >= s_RData.MaxIndices) 
		{
			Submit();
			s_RData.IndexCount = 0;
			s_RData.VertexIndex = 0;
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			s_RData.VertexArrayPtr[s_RData.VertexIndex].Position = transform * s_RData.VertexPositions[i];
			s_RData.VertexArrayPtr[s_RData.VertexIndex].Color = color;
			s_RData.VertexIndex++;
		}

		s_RData.IndexCount += 6;
	}

	void BatchRenderer::EndScene()
	{
		Submit();
	}

	void BatchRenderer::Submit()
	{
		if (s_RData.IndexCount == 0)
			return;

		s_RData.VertexBuffer->SetData(s_RData.VertexArrayPtr, s_RData.VertexIndex * sizeof(Vertex));

		Renderer::Draw(s_RData.VertexArray, s_RData.IndexCount);
	}
}