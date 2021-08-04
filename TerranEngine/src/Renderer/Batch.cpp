#include "trpch.h"
#include "Batch.h"

#include "RenderCommand.h"

#include <glad/glad.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace TerranEngine 
{
	glm::vec4 Batch::s_VertexPositions[4];
	int* Batch::s_Indices;
	IndexBuffer* Batch::s_IndexBuffer;

	void decomposeMatrix(const glm::mat4& m, glm::vec3& pos, glm::quat& rot, glm::vec3& scale)
	{
		pos = m[3];
		for (int i = 0; i < 3; i++)
			scale[i] = glm::length(glm::vec3(m[i]));
		const glm::mat3 rotMtx(
			glm::vec3(m[0]) / scale[0],
			glm::vec3(m[1]) / scale[1],
			glm::vec3(m[2]) / scale[2]);
		rot = glm::quat_cast(rotMtx);
	}

	void createTransformMatrix(glm::mat4& m, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale) 
	{
		m = glm::translate(glm::mat4(1.0f), pos) *
			glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), scale);
	}

	BatchData Batch::InitData(uint32_t batchSize, uint32_t zIndex, Shader* shader)
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

		if (s_Indices == nullptr) 
		{
			s_Indices = new int[data.MaxIndices];

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

		s_IndexBuffer = new IndexBuffer(s_Indices, data.MaxIndices * sizeof(int));

		int sampler[data.MaxTextureSlots];

		for (size_t i = 0; i < data.MaxTextureSlots; i++)
			sampler[i] = i;

		data.Shader = shader;
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

		// note: shit fix, should probably use shared pointer
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

	void Batch::AddQuad(BatchData& data, glm::mat4& transform, const glm::vec4& color, Texture* texture, glm::vec2 textureCoordinates[4])
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
			data.VertexPtr[data.VertexPtrIndex].TextureCoordinates = textureCoordinates[i];
			data.VertexPtr[data.VertexPtrIndex].TextureIndex = texIndex;

			data.VertexPtrIndex++;
		}

		data.IndexCount += 6;
	}

	void Batch::AddText(BatchData& data, glm::mat4& transform, const glm::vec4& color, Font* font, const std::string& text)
	{
		float texIndex = 0.0f;

		for (size_t i = 1; i < data.TextureIndex; i++)
		{
			if (data.Textures[i] == font->GetTexutre())
			{
				texIndex = i;
				break;
			}
		}

		if (texIndex == 0.0f)
		{
			texIndex = data.TextureIndex;
			data.Textures[data.TextureIndex] = font->GetTexutre();
			data.TextureIndex++;
		}


		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;


		decomposeMatrix(transform, position, rotation, scale);
		for (size_t i = 0; i < text.size(); i++)
		{
			char c = text[i];

			ftgl::texture_glyph_t* glyph = font->LoadGlyph(c);


			if (glyph != NULL) 
			{
				glm::vec2 uvs[4] =
				{
					glm::vec2(glyph->s0, glyph->t0),
					glm::vec2(glyph->s0, glyph->t1),
					glm::vec2(glyph->s1, glyph->t1),
					glm::vec2(glyph->s1, glyph->t0),
				};
				
				glm::vec3 pos = glm::vec3(
					position.x + glyph->offset_x,
					position.y + glyph->offset_y, 0.0f);


				glm::vec3 size = glm::vec3(
					pos.x + glyph->width,
					pos.y - glyph->height, 0.0f
				);

				createTransformMatrix(transform, pos, rotation, size);

				for (size_t i = 0; i < 4; i++)
				{
					data.VertexPtr[data.VertexPtrIndex].Position = transform * s_VertexPositions[i];
					data.VertexPtr[data.VertexPtrIndex].Color = color;
					data.VertexPtr[data.VertexPtrIndex].TextureCoordinates = uvs[i];
					data.VertexPtr[data.VertexPtrIndex].TextureIndex = texIndex;

					data.VertexPtrIndex++;
				}

				data.IndexCount += 6;

				position.x += glyph->advance_x / 14.0f;
			}
		}
	}

	void Batch::EndScene(BatchData& data)
	{
		if (data.IndexCount == 0)
			return;
		
		for (size_t i = 0; i < data.TextureIndex; i++)
			data.Textures[i]->Bind(i);

		data.VertexArray->Bind();
		data.VertexBuffer->SetData(data.VertexPtr, data.VertexPtrIndex * sizeof(Vertex));
		s_IndexBuffer->Bind();

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
