#include "trpch.h"
#include "BatchRenderer.h"

#include <glm/vec3.hpp>


#include <glad/glad.h>

namespace TerranEngine 
{
	Data BatchRenderer::s_Data;

	void BatchRenderer::Init(uint32_t batchSize)
	{
		s_Data.MaxVertices = batchSize * 4;
		s_Data.MaxIndices = batchSize * 6;

		s_Data.VertexArrayPtr = new Vertex[s_Data.MaxVertices];

		s_Data.VertexArray = new VertexArray();

		s_Data.VertexBuffer = new VertexBuffer(s_Data.MaxVertices * sizeof(Vertex));

		s_Data.VertexArray->AddVertexBufferLayout({
			{ GL_FLOAT, 3 },
			{ GL_FLOAT, 4 },
			{ GL_FLOAT, 2 },
			{ GL_FLOAT, 1 }
		});

		int* indices = new int[s_Data.MaxIndices];

		uint32_t offset = 0;

		for (size_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}

		s_Data.IndexBuffer = new IndexBuffer(indices, s_Data.MaxIndices * sizeof(int));

		delete[] indices;
		
		int samplers[s_Data.MaxTextureSlots];

		for (size_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		uint32_t whiteTexture = 0xffffffff;
		s_Data.WhiteTexture = new Texture(1, 1);
		s_Data.WhiteTexture->SetData(&whiteTexture);

		s_Data.shader = new Shader("res/VertexShader.glsl", "res/FragmentShader.glsl");
		s_Data.shader->UploadIntArray("u_Samplers", s_Data.MaxTextureSlots, samplers);


		s_Data.Textures[0] = s_Data.WhiteTexture;

		s_Data.VertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.VertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.VertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.VertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data.TextureCoordinates[0] = { 0.0f, 0.0f };
		s_Data.TextureCoordinates[1] = { 1.0f, 0.0f };
		s_Data.TextureCoordinates[2] = { 1.0f, 1.0f };
		s_Data.TextureCoordinates[3] = { 0.0f, 1.0f };
	}

	void BatchRenderer::Close()
	{
		delete[] s_Data.VertexArrayPtr;
		delete s_Data.VertexBuffer;
		delete s_Data.IndexBuffer;
		delete s_Data.VertexArray;

		delete s_Data.shader;

		delete s_Data.WhiteTexture;
	}
	
	void BatchRenderer::BeginScene(Camera& camera, const glm::mat4& transform)
	{
		s_Data.shader->UploadMat4("u_ProjMat", camera.GetProjection());
		s_Data.shader->UploadMat4("u_ViewMat", glm::inverse(transform));

		s_Data.IndexCount = 0;
		s_Data.VertexArrayIndex = 0;
		s_Data.TexIndex = 1;
	}
	
	void BatchRenderer::EndScene()
	{
		Submit();
	}
	
	void BatchRenderer::Draw(const glm::mat4& transform, const glm::vec4& color)
	{
		if (s_Data.IndexCount >= s_Data.MaxIndices)
			BeginNewBatch();

		for (size_t i = 0; i < 4; i++)
		{
			s_Data.VertexArrayPtr[s_Data.VertexArrayIndex].Position = transform * s_Data.VertexPositions[i];
			s_Data.VertexArrayPtr[s_Data.VertexArrayIndex].Color = color;
			s_Data.VertexArrayPtr[s_Data.VertexArrayIndex].TextureCoords = s_Data.TextureCoordinates[i];
			s_Data.VertexArrayPtr[s_Data.VertexArrayIndex].TextureIndex = 0.0f;

			s_Data.VertexArrayIndex++;
		}

		s_Data.IndexCount += 6;
	}

	void BatchRenderer::Draw(const glm::mat4& transform, const glm::vec4& color, Texture& texture) 
	{
		float texIndex = 0.0f;

		for (size_t i = 1; i < s_Data.TexIndex; i++)
		{
			if (s_Data.Textures[i] == &texture)
			{
				texIndex = i;
				break;
			}
		}

		if (texIndex == 0.0f)
		{
			if (s_Data.TexIndex >= s_Data.MaxTextureSlots)
				BeginNewBatch();

			texIndex = s_Data.TexIndex;
			s_Data.Textures[s_Data.TexIndex] = &texture;
			s_Data.TexIndex++;
		}

		if (s_Data.IndexCount >= s_Data.MaxIndices)
			BeginNewBatch();

		for (size_t i = 0; i < 4; i++)
		{
			s_Data.VertexArrayPtr[s_Data.VertexArrayIndex].Position = transform * s_Data.VertexPositions[i];
			s_Data.VertexArrayPtr[s_Data.VertexArrayIndex].Color = color;
			s_Data.VertexArrayPtr[s_Data.VertexArrayIndex].TextureCoords = s_Data.TextureCoordinates[i];
			s_Data.VertexArrayPtr[s_Data.VertexArrayIndex].TextureIndex = texIndex;

			s_Data.VertexArrayIndex++;
		}

		s_Data.IndexCount += 6;
	}

	void BatchRenderer::BeginNewBatch()
	{
		s_Data.IndexCount = 0;
		s_Data.VertexArrayIndex = 0;
		s_Data.TexIndex = 1;
		Submit();
	}
	
	void BatchRenderer::Submit()
	{
		if (s_Data.IndexCount == 0)
			return;

		s_Data.VertexBuffer->SetData(s_Data.VertexArrayPtr, s_Data.VertexArrayIndex * sizeof(Vertex));

		for (size_t i = 0; i < s_Data.TexIndex; i++)
			s_Data.Textures[i]->Bind(i);

		Renderer::Draw(*s_Data.VertexArray, s_Data.IndexCount);
	}
}