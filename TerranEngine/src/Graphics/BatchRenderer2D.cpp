#include "trpch.h"
#include "BatchRenderer2D.h"

#include "RenderCommand.h"

#include <glad/glad.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#pragma warning (push)
#pragma warning (disable : 4305)
#pragma warning (disable : 4309)

namespace TerranEngine 
{
	BatchRenderer2D* BatchRenderer2D::m_Instance;

	static void decomposeMatrix(const glm::mat4& m, glm::vec3& pos, glm::quat& rot, glm::vec3& scale)
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

	static void createTransformMatrix(glm::mat4& m, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& scale) 
	{
		m = glm::translate(glm::mat4(1.0f), pos) *
			glm::rotate(glm::mat4(1.0f), rot.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), scale);
	}

	BatchRenderer2D::BatchRenderer2D(uint32_t batchSize)
	{
		m_Instance = this;
		Init(batchSize);
	}

	BatchRenderer2D::~BatchRenderer2D()
	{
		Close();
	}

	void BatchRenderer2D::Init(uint32_t batchSize)
	{
		m_MaxIndices = batchSize * 6;
		m_MaxVertices = batchSize * 4;

		m_Stats.MaxIndices = m_MaxIndices;
		m_Stats.MaxVertices = m_MaxVertices;

		// ******** Index buffer setup ********
		int* indices = new int[m_MaxIndices];

		uint32_t offset = 0;

		for (uint32_t i = 0; i < batchSize * 6; i += 6)
		{
			indices[i] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}

		m_IndexBuffer = CreateShared<IndexBuffer>(indices, m_MaxIndices * sizeof(int));

		delete[] indices;
		// ************************************

		TextureParameters parameters;

		uint16_t whiteTextureData = 0xffffffff;

		int maxTexSlots = 16; //RenderCommand::GetMaxTextureSlots();

		// ******** Quad ******** 
		{
			m_QuadVertexPtr = new QuadVertex[m_MaxVertices];

			m_QuadVAO = CreateShared<VertexArray>();

			m_QuadVBO = CreateShared<VertexBuffer>(m_MaxVertices * sizeof(QuadVertex));

			m_QuadVAO->AddVertexBufferLayout({
				{ GL_FLOAT, 3 },
				{ GL_FLOAT, 4 },
				{ GL_FLOAT, 2 },
				{ GL_FLOAT, 1 }
			});

			m_QuadVAO->AddIndexBuffer(m_IndexBuffer);

			int* sampler = new int[maxTexSlots];

			for (size_t i = 0; i < maxTexSlots; i++)
				sampler[i] = i;

			m_QuadShader = CreateShared<Shader>("DefaultQuadShader", "res/shaders/Base/Quad/QuadVertex.glsl", "res/shaders/Base/Quad/QuadFragment.glsl");

			m_QuadShader->UploadIntArray("u_Samplers", maxTexSlots, sampler);
			m_QuadShader->Unbind();

			m_QuadTextures[0] =	CreateShared<Texture>(1, 1, parameters);
			m_QuadTextures[0]->SetData(&whiteTextureData);

			delete[] sampler;

		}
		// **********************


		// ******** Text ******** 
		{
			m_TextVertexPtr = new QuadVertex[m_MaxVertices];

			m_TextVAO = CreateShared<VertexArray>();

			m_TextVBO = CreateShared<VertexBuffer>(m_MaxVertices * sizeof(QuadVertex));

			m_TextVAO->AddVertexBufferLayout({
				{ GL_FLOAT, 3 },
				{ GL_FLOAT, 4 },
				{ GL_FLOAT, 2 },
				{ GL_FLOAT, 1 }
			});

			m_TextVAO->AddIndexBuffer(m_IndexBuffer);


			int* sampler = new int[maxTexSlots];

			for (size_t i = 0; i < maxTexSlots; i++)
				sampler[i] = i;

			m_TextShader = CreateShared<Shader>("DefaultTextShader", "res/shaders/Base/Text/TextVertex.glsl", "res/shaders/Base/Text/TextFragment.glsl");

			m_TextShader->UploadIntArray("u_Samplers", maxTexSlots, sampler);

			m_TextShader->Unbind();

			m_TextTextures[0] = CreateShared<Texture>(1, 1, parameters);
			m_TextTextures[0]->SetData(&whiteTextureData);

			delete[] sampler;
		}
		// **********************


		// ******** Circle ********
		{
			m_CircleVertexPtr = new CircleVertex[m_MaxVertices];

			m_CircleVAO = CreateShared<VertexArray>();

			m_CircleVBO = CreateShared<VertexBuffer>(m_MaxVertices * sizeof(CircleVertex));

			m_CircleVAO->AddVertexBufferLayout({
				{ GL_FLOAT, 3 },
				{ GL_FLOAT, 1 },
				{ GL_FLOAT, 4 },
				{ GL_FLOAT, 2 }
			});

			m_CircleVAO->AddIndexBuffer(m_IndexBuffer);

			m_CircleShader = CreateShared<Shader>("DefaultCircleShader", "res/shaders/Base/Circle/CircleVertex.glsl", "res/shaders/Base/Circle/CircleFragment.glsl");

			m_CircleShader->Unbind();
		}
		// ************************


		m_VertexPositions[0] = { -1.0f, -1.0f, 0.0f, 1.0f };
		m_VertexPositions[1] = {  1.0f, -1.0f, 0.0f, 1.0f };
		m_VertexPositions[2] = {  1.0f,  1.0f, 0.0f, 1.0f };
		m_VertexPositions[3] = { -1.0f,  1.0f, 0.0f, 1.0f };


		m_QuadShader->Bind();
		m_TextShader->Bind();
		m_CircleShader->Bind();

		m_CameraBuffer = CreateShared<UniformBuffer>(sizeof(CameraData), 0);

		m_QuadShader->Unbind();
		m_TextShader->Unbind();
		m_CircleShader->Unbind();

	}

	void BatchRenderer2D::Close()
	{
		delete[] m_QuadVertexPtr;
	}

	void BatchRenderer2D::BeginScene(Camera& camera, const glm::mat4& transform)
	{
		Clear();

		m_QuadShader->Bind();
		m_TextShader->Bind();
		m_CircleShader->Bind();

		m_CameraData.Projection = camera.GetProjection();
		m_CameraData.View = glm::inverse(transform);

		m_CameraData.ProjectionSize = { 0, 0, 0 };
		m_CameraData.CameraPosition = transform[3];

		m_CameraBuffer->Bind();
		m_CameraBuffer->SetData(&m_CameraData, 0, sizeof(CameraData) - (3 * sizeof(float)));

		m_QuadShader->Unbind();
		m_TextShader->Unbind();
		m_CircleShader->Unbind();
	}

	void BatchRenderer2D::AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture)
	{
		glm::vec2 textureCoords[4] = 
		{
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
		};

		AddQuad(transform, color, texture, textureCoords);
	}

	void BatchRenderer2D::AddQuad(glm::mat4& transform, const glm::vec4& color)
	{
		AddQuad(transform, color, nullptr);
	}

	void BatchRenderer2D::AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture, glm::vec2 textureCoordinates[4])
	{
		if (!InCameraView(transform))
			return;

		if (!QuadBatchHasRoom()) 
		{
			// Begin New Batch
			EndScene();
			Clear();
		}

		float texIndex = -1;

		if (texture == nullptr)
			texIndex = 0.0f;

		for (size_t i = 1; i < m_QuadTextureIndex; i++)
		{
			if (m_QuadTextures[i] == texture)
			{
				texIndex = i;
				break;
			}
		}

		if (texIndex == -1)
		{
			texIndex = m_QuadTextureIndex;
			m_QuadTextures[m_QuadTextureIndex] = texture;
			m_QuadTextureIndex++;
		}

		for (size_t i = 0; i < 4; i++)
		{
			m_QuadVertexPtr[m_QuadVertexPtrIndex].Position = transform * m_VertexPositions[i];
			m_QuadVertexPtr[m_QuadVertexPtrIndex].Color = color;
			m_QuadVertexPtr[m_QuadVertexPtrIndex].TextureCoordinates = textureCoordinates[i];
			m_QuadVertexPtr[m_QuadVertexPtrIndex].TextureIndex = texIndex;

			m_QuadVertexPtrIndex++;
		}

		m_QuadIndexCount += 6;
	}

	void BatchRenderer2D::AddText(glm::mat4& transform, const glm::vec4& color, Shared<Font> font, const std::string& text)
	{

		/* Note: there's there's some fuckery with displaying the characters 
		* which i think is related to how freetype-gl stores them in a packed texture
		* 
		* fix fucker
		*/

		if (!InCameraView(transform))
			return;

		if (!TextBatchHasRoom()) 
		{
			// Begin new batch
			EndScene();
			Clear();
		}

		float texIndex = -1;

		if (font == nullptr)
			texIndex = 0.0f;

		for (size_t i = 1; i < m_TextTextureIndex; i++)
		{
			if (m_TextTextures[i] == font->GetTexutre())
			{
				texIndex = i;
				break;
			}
		}

		if (texIndex == -1)
		{
			texIndex = m_TextTextureIndex;
			m_TextTextures[m_TextTextureIndex] = font->GetTexutre();
			m_TextTextureIndex++;
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
					m_TextVertexPtr[m_TextVertexPtrIndex].Position = transform * vertexPositions[i];
					m_TextVertexPtr[m_TextVertexPtrIndex].Color = color;
					m_TextVertexPtr[m_TextVertexPtrIndex].TextureCoordinates = uvs[i];
					m_TextVertexPtr[m_TextVertexPtrIndex].TextureIndex = texIndex;

					m_TextVertexPtrIndex++;
				}

				m_TextIndexCount += 6;

				position.x += glyph->advance_x / 80.0f;
			}
		}
	}

	void BatchRenderer2D::AddCircle(glm::mat4& transform, const glm::vec4& color, float thickness)
	{

		//if (!InCameraView(transform))
		//	return;

		if (!CircleBatchHasRoom())
		{
			// Begin New Batch
			EndScene();
			Clear();
		}

		for (size_t i = 0; i < 4; i++)
		{
			m_CircleVertexPtr[m_CircleVertexPtrIndex].Position = transform * m_VertexPositions[i];
			m_CircleVertexPtr[m_CircleVertexPtrIndex].Thickness = thickness;
			m_CircleVertexPtr[m_CircleVertexPtrIndex].Color = color;
			m_CircleVertexPtr[m_CircleVertexPtrIndex].LocalPosition = glm::vec2(m_VertexPositions[i].x, m_VertexPositions[i].y);

			m_CircleVertexPtrIndex++;
		}

		m_CircleIndexCount += 6;
	}

	void BatchRenderer2D::EndScene()
	{
		m_Stats.VertexCount += m_QuadVertexPtrIndex + m_TextVertexPtrIndex + m_CircleVertexPtrIndex;
		m_Stats.IndexCount +=  m_QuadIndexCount + m_TextIndexCount + m_CircleIndexCount;

		// Submit quads
		if (m_QuadIndexCount)
		{

			m_QuadShader->Bind();
			m_QuadVAO->Bind();
			m_QuadVBO->SetData(m_QuadVertexPtr, m_QuadVertexPtrIndex * sizeof(QuadVertex));

			for (size_t i = 0; i < m_QuadTextureIndex; i++)
				m_QuadTextures[i]->Bind(i);

			RenderCommand::Draw(RenderMode::Triangles, m_QuadVAO, m_QuadIndexCount);

			m_Stats.DrawCalls++;

			m_QuadShader->Unbind();
		}

		// Submit text
		if (m_TextIndexCount) 
		{
			m_TextShader->Bind();
			m_TextVAO->Bind();
			m_TextVBO->SetData(m_TextVertexPtr, m_TextVertexPtrIndex * sizeof(QuadVertex));


			for (size_t i = 0; i < m_TextTextureIndex; i++)
				m_TextTextures[i]->Bind(i);

			RenderCommand::Draw(RenderMode::Triangles, m_TextVAO, m_TextIndexCount);

			m_Stats.DrawCalls++;

			m_TextShader->Unbind();
		}


		// Submit circles
		if (m_CircleIndexCount) 
		{
			m_CircleShader->Bind();
			m_CircleVAO->Bind();
			m_CircleVBO->SetData(m_CircleVertexPtr, m_CircleVertexPtrIndex * sizeof(CircleVertex));

			RenderCommand::Draw(RenderMode::Triangles, m_CircleVAO, m_CircleIndexCount);

			m_Stats.DrawCalls++;

			m_CircleShader->Unbind();
		}
	}

	void BatchRenderer2D::Clear()
	{
		m_QuadVertexPtrIndex = 0;
		m_QuadIndexCount = 0;
		m_QuadTextureIndex = 1;

		m_TextVertexPtrIndex = 0;
		m_TextIndexCount = 0;
		m_TextTextureIndex = 1;

		m_CircleVertexPtrIndex = 0;
		m_CircleIndexCount = 0;
	}

	// NOTE: This is (for now) only for orthographic cameras

	bool BatchRenderer2D::InCameraViewX(float x, float width) 
	{
		if ((x + (width / 2) + 0.5f >= m_CameraData.CameraPosition.x + -m_CameraData.ProjectionSize.x / 2 && x - (width / 2) - 0.5f <= m_CameraData.CameraPosition.x + m_CameraData.ProjectionSize.x / 2))
			return true;

		return false;
	}

	bool BatchRenderer2D::InCameraViewY(float y, float height)
	{
		if ((y + (height / 2) + 0.5f >= m_CameraData.CameraPosition.y + -m_CameraData.ProjectionSize.y / 2 && y - (height / 2) - 0.5f <= m_CameraData.CameraPosition.y + m_CameraData.ProjectionSize.y / 2))
			return true;

		return false;
	}
	
	bool BatchRenderer2D::InCameraViewZ(float z, float depth)
	{
		if ((z + (depth / 2) + 0.5f >= m_CameraData.CameraPosition.z + -m_CameraData.ProjectionSize.z / 2 && z - (depth / 2) - 0.5f <= m_CameraData.CameraPosition.z + m_CameraData.ProjectionSize.z / 2))
			return true;

		return false;
	}

	bool BatchRenderer2D::InCameraView(glm::mat4& transform)
	{
		glm::vec3 position, size;
		glm::quat rotation;

		decomposeMatrix(transform, position, rotation, size);

		if (InCameraViewX(position.x, size.x) && InCameraViewY(position.y, size.y) && InCameraViewZ(position.z, size.z))
			return true;

		return m_CullObjectsOutsideOfCamera ? false : true;
	}
}
#pragma warning (pop)
