#include "trpch.h"
#include "Batch.h"

#include "RenderCommand.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	glm::vec4 Batch::s_VertexPositions[4];
	int* Batch::s_Indices;

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
			{ GL_FLOAT, 1 }
		});

		if (s_Indices == nullptr) 
		{
			TR_TRACE("intited indices");
			s_Indices = new int[batchSize * 6];

			uint32_t offset = 0;

			for (size_t i = 0; i < batchSize * 6; i += 6)
			{
				s_Indices[i + 0] = offset + 0;
				s_Indices[i + 1] = offset + 1;
				s_Indices[i + 2] = offset + 2;
				s_Indices[i + 3] = offset + 2;
				s_Indices[i + 4] = offset + 3;
				s_Indices[i + 5] = offset + 0;

				offset += 4;
			}

		}

		data.IndexBuffer = new IndexBuffer(s_Indices, data.MaxIndices * sizeof(int));

		int sampler[data.MaxTextureSlots];

		for (size_t i = 0; i < data.MaxTextureSlots; i++)
			sampler[i] = i;

		data.Shader = new Shader("res/VertexShader.glsl", "res/FragmentShader.glsl");
		data.Shader->UploadIntArray("u_Samplers", data.MaxTextureSlots, sampler);

		uint16_t whiteTextureData = 0xffffffff;

		data.Textures[0] = new Texture(1, 1);
		data.Textures[0]->SetData(&whiteTextureData);

		s_VertexPositions[0] = { -1.0f, -1.0f, 0.0f, 1.0f };
		s_VertexPositions[1] = {  1.0f, -1.0f, 0.0f, 1.0f };
		s_VertexPositions[2] = {  1.0f,  1.0f, 0.0f, 1.0f };
		s_VertexPositions[3] = { -1.0f,  1.0f, 0.0f, 1.0f };

		return data;
	}

	void Batch::CloseData(BatchData& data)
	{
		delete[] data.VertexPtr;

		// note: shit fix should probably use shared pointer
		if (s_Indices != nullptr) 
		{
			delete[] s_Indices;
			s_Indices = nullptr;
		}
	}

	void Batch::BeginScene(BatchData& data, Camera& camera, const glm::mat4& transform)
	{
		data.Shader->UploadMat4("u_ProjMat", camera.GetProjection());
		data.Shader->UploadMat4("u_ViewMat", glm::inverse(transform));
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
			data.VertexPtr[data.VertexPtrIndex].Position = transform * s_VertexPositions[i];
			data.VertexPtr[data.VertexPtrIndex].Color = color;
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

