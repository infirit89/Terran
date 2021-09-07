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
			glm::rotate(glm::mat4(1.0f), glm::degrees(rot.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), scale);
	}

	BatchData Batch::InitData(uint32_t batchSize, Shader* shader)
	{
		BatchData data;

		data.MaxIndices = batchSize * 6;
		data.MaxVertices = batchSize * 4;

		data.BatchStats.MaxIndices = data.MaxIndices;
		data.BatchStats.MaxVertices = data.MaxVertices;

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

		data.CameraBuffer = new UniformBuffer(sizeof(BatchData::CameraData), 0);

		return data;
	}

	void Batch::CloseData(BatchData& data)
	{
		delete[] data.VertexPtr;

		delete data.VertexArray;
		delete data.VertexBuffer;
		delete data.CameraBuffer;

		// note: shit fix, should probably use shared pointer
		if (s_Indices != nullptr) 
		{
			delete[] s_Indices;
			s_Indices = nullptr;
		}
	}

	void Batch::BeginScene(BatchData& data, Camera& camera, const glm::mat4& transform)
	{
		data.Shader->Bind();

		data.CameraData.projection = camera.GetProjection();
		data.CameraData.view = glm::inverse(transform);

		data.CameraBuffer->SetData(&data.CameraData, sizeof(BatchData::CameraData));
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

		/* Note: there's there's some fuckery with displaying the characters 
		* which i think is related to how freetype - gl stores them in a packed texture
		* 
		* fix fucker
		*/
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
		glm::vec3 scale;
		glm::quat rotation;

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
					glm::vec2(glyph->s1, glyph->t0),
					glm::vec2(glyph->s1, glyph->t1),
					glm::vec2(glyph->s0, glyph->t1),
				};

				if (i > 0)
				{
					float kerning = ftgl::texture_glyph_get_kerning(glyph, std::string(1, text[i - 1]).c_str());
					position.x += kerning;
				}

				float x0 = position.x + (glyph->offset_x / 80.0f);
				float y0 = position.y + (glyph->offset_y / 80.0f);
				float x1 = x0 + glyph->width / 80.0f;
				float y1 = y0 - glyph->height / 80.0f;

				glm::vec4 vertexPositions[4] =
				{
					glm::vec4(x0, y0, 0.0f, 1.0f),
					glm::vec4(x1, y0, 0.0f, 1.0f),
					glm::vec4(x1, y1, 0.0f, 1.0f),
					glm::vec4(x0, y1, 0.0f, 1.0f),
				};

				for (size_t i = 0; i < 4; i++)
				{
					data.VertexPtr[data.VertexPtrIndex].Position = transform * vertexPositions[i];
					data.VertexPtr[data.VertexPtrIndex].Color = color;
					data.VertexPtr[data.VertexPtrIndex].TextureCoordinates = uvs[i];
					data.VertexPtr[data.VertexPtrIndex].TextureIndex = texIndex;

					data.VertexPtrIndex++;
				}

				data.IndexCount += 6;

				position.x += glyph->advance_x / 80.0f;
			}
		}
	}

	void Batch::EndScene(BatchData& data)
	{
		if (data.IndexCount == 0)
			return;
		
		data.BatchStats.VertexCount = data.VertexPtrIndex;
		data.BatchStats.IndexCount = data.IndexCount;

		data.Shader->Bind();

		for (size_t i = 0; i < data.TextureIndex; i++)
			data.Textures[i]->Bind(i);

		data.VertexArray->Bind();
		s_IndexBuffer->Bind();
		data.VertexBuffer->SetData(data.VertexPtr, data.VertexPtrIndex * sizeof(Vertex));

		RenderCommand::Draw(*data.VertexArray, data.IndexCount);

		data.Shader->Unbind();
		data.VertexArray->Unbind();
		data.VertexBuffer->Unbind();
	}

	void Batch::Clear(BatchData& data)
	{
		data.VertexPtrIndex = 0;
		data.IndexCount = 0;
		data.TextureIndex = 1;
	}

	void Batch::ResetStats(BatchData& data)
	{
		memset(&data.BatchStats, 0, sizeof(BatchStats));
	}


}
