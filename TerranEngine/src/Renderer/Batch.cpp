#include "trpch.h"
#include "Batch.h"

#include "RenderCommand.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	glm::vec4 Batch::VertexPositions[4];
	glm::vec2 Batch::TextureCoordinates[4];

	BatchData Batch::InitData(uint32_t batchSize, uint32_t zIndex)
	{
		BatchData data;

		data.ZIndex = zIndex;

		data.MaxIndices = batchSize * 6;
		data.MaxVertices = batchSize * 4;

		data.VertexPtr = new Vertex[data.MaxVertices];

		data.VertexArray = new VertexArray();

		data.VertexBuffer = new VertexBuffer(data.MaxVertices * sizeof(Vertex));

		data.VertexArray->AddVertexBufferLayout({
			{ GL_FLOAT, 3 },
			{ GL_FLOAT, 4 },
			{ GL_FLOAT, 2 },
			{ GL_FLOAT, 1 }
		});

		int* indices = new int[data.MaxIndices];

		uint32_t offset = 0;

		for (size_t i = 0; i < data.MaxIndices; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}

		data.IndexBuffer = new IndexBuffer(indices, data.MaxIndices * sizeof(int));

		delete[] indices;

		int sampler[data.MaxTextureSlots];

		for (size_t i = 0; i < data.MaxTextureSlots; i++)
			sampler[i] = i;

		data.Shader = new Shader("res/VertexShader.glsl", "res/FragmentShader.glsl");
		data.Shader->UploadIntArray("u_Samplers", data.MaxTextureSlots, sampler);

		uint16_t whiteTextureData = 0xffffffff;

		data.Textures[0] = new Texture(1, 1);
		data.Textures[0]->SetData(&whiteTextureData);

		VertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		VertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		VertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		VertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		TextureCoordinates[0] = { 0.0f, 0.0f };
		TextureCoordinates[1] = { 1.0f, 0.0f };
		TextureCoordinates[2] = { 1.0f, 1.0f };
		TextureCoordinates[3] = { 0.0f, 1.0f };

		return data;
	}

	void Batch::CloseData(BatchData& data)
	{
		delete[] data.VertexPtr;
	}

	void Batch::BeginScene(BatchData& data, Camera& camera, const glm::mat4& transform)
	{
		data.Shader->UploadMat4("u_ProjMat", camera.GetProjection());
		data.Shader->UploadMat4("u_ViewMat", transform);
	}

	void Batch::AddQuad(BatchData& data, const glm::mat4& transform, const glm::vec4& color, Texture* texture)
	{
		float texIndex = -1;

		if (texture == nullptr)
			texIndex = 0.0f;

		for (size_t i = 1; i < data.TextureIndex; i++)
		{
			if (data.Textures[i] == texture) 
			{
				texIndex = i;
				break;
			}
		}

		if (texIndex == -1)
		{
			texIndex = data.TextureIndex;
			data.Textures[data.TextureIndex] = texture;
			data.TextureIndex++;
		}

		for (size_t i = 0; i < 4; i++)
		{
			data.VertexPtr[data.VertexPtrIndex].Position = transform * VertexPositions[i];
			data.VertexPtr[data.VertexPtrIndex].Color = color;
			data.VertexPtr[data.VertexPtrIndex].TextureCoords = TextureCoordinates[i];
			data.VertexPtr[data.VertexPtrIndex].TextureIndex = texIndex;

			data.VertexPtrIndex++;
		}

		data.IndexCount += 6;
	}

	void Batch::EndScene(BatchData& data)
	{
		if (data.IndexCount == 0)
			return;
		
		for (size_t i = 0; i < data.TextureIndex; i++)
			data.Textures[i]->Bind(i);

		data.VertexArray->Bind();
		data.VertexBuffer->SetData(data.VertexPtr, data.VertexPtrIndex * sizeof(Vertex));
		data.IndexBuffer->Bind();

		data.Shader->Bind();

		RenderCommand::Draw(*data.VertexArray, data.IndexCount);
	}

	void Batch::Clear(BatchData& data)
	{
		data.VertexPtrIndex = 0;
		data.IndexCount = 0;
		data.TextureIndex = 1;
	}
}

