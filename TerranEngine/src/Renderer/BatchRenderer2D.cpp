#include "trpch.h"
#include "BatchRenderer2D.h"

#include "RenderCommand.h"

#include <glad/glad.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace TerranEngine 
{
	BatchRenderer2D* BatchRenderer2D::m_Instance;

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

	void BatchRenderer2D::CreateFramebuffer(uint32_t width, uint32_t height, bool swapchainTarget)
	{
		m_Framebuffer = CreateShared<Framebuffer>(width, height, swapchainTarget);

		float pos[] =
		{
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f
		};
		m_FramebufferVAO = CreateShared<VertexArray>();

		m_FramebufferVBO = CreateShared<VertexBuffer>(pos, sizeof(pos));

		m_FramebufferVAO->AddVertexBufferLayout({
			{ GL_FLOAT, 3 },
			{ GL_FLOAT, 2 },
		});

		int indices[] =
		{
			0, 1, 2,
			2, 3, 0
		};

		m_FramebufferIBO = CreateShared<IndexBuffer>(indices, sizeof(indices));

		m_FramebufferVAO->Unbind();
		m_FramebufferVBO->Unbind();
		m_FramebufferIBO->Unbind();

		m_Shader->Unbind();
		m_FramebufferShader = CreateShared<Shader>("res/shaders/Framebuffer/Framebuffershader.glsl");
		m_FramebufferShader->Unbind();
		
		m_Shader->Bind();
	}

	void BatchRenderer2D::Init(uint32_t batchSize)
	{
		m_MaxIndices = batchSize * 6;
		m_MaxVertices = batchSize * 4;

		m_Stats.MaxIndices = m_MaxIndices;
		m_Stats.MaxVertices = m_MaxVertices;

		m_VertexPtr = new Vertex[m_MaxVertices];

		m_VertexArray = CreateShared<VertexArray>();

		m_VertexBuffer = CreateShared<VertexBuffer>(m_MaxVertices * sizeof(Vertex));

		m_VertexArray->AddVertexBufferLayout({
			{ GL_FLOAT, 3 },
			{ GL_FLOAT, 4 },
			{ GL_FLOAT, 2 },
			{ GL_FLOAT, 1 }
		});

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

		int sampler[m_MaxTextureSlots];

		for (size_t i = 0; i < m_MaxTextureSlots; i++)
			sampler[i] = i;

		m_Shader = CreateShared<Shader>("DefaultShader", "res/shaders/VertexShader.glsl", "res/shaders/FragmentShader.glsl");
		m_Shader->UploadIntArray("u_Samplers", m_MaxTextureSlots, sampler);

		uint16_t whiteTextureData = 0xffffffff;

		m_Textures[0] = CreateShared<Texture>(1, 1);
		m_Textures[0]->SetData(&whiteTextureData);

		m_VertexPositions[0] = { -1.0f, -1.0f, 0.0f, 1.0f };
		m_VertexPositions[1] = {  1.0f, -1.0f, 0.0f, 1.0f };
		m_VertexPositions[2] = {  1.0f,  1.0f, 0.0f, 1.0f };
		m_VertexPositions[3] = { -1.0f,  1.0f, 0.0f, 1.0f };

		m_CameraBuffer = CreateShared<UniformBuffer>(sizeof(CameraData), 0);
	}

	void BatchRenderer2D::Close()
	{
		delete[] m_VertexPtr;
	}

	void BatchRenderer2D::BeginScene(Camera& camera, const glm::mat4& transform)
	{
		Clear();

		//m_Shader->Bind();

		m_CameraData.Projection = camera.GetProjection();
		m_CameraData.View = glm::inverse(transform);

		m_CameraData.ProjectionSize = { camera.GetWidth(), camera.GetHeight(), camera.GetDepth() };
		m_CameraData.CameraPosition = transform[3];

		m_CameraBuffer->Bind();
		m_CameraBuffer->SetData(&m_CameraData, sizeof(CameraData) - (3 * sizeof(float)));
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

		if (!HasRoom()) 
		{
			// Begin New Batch
			EndScene();
			Clear();
		}

		float texIndex = -1;

		if (texture == nullptr)
			texIndex = 0.0f;

		for (size_t i = 1; i < m_TextureIndex; i++)
		{
			if (m_Textures[i] == texture)
			{
				texIndex = i;
				break;
			}
		}

		if (texIndex == -1)
		{
			texIndex = m_TextureIndex;
			m_Textures[m_TextureIndex] = texture;
			m_TextureIndex++;
		}

		for (size_t i = 0; i < 4; i++)
		{
			m_VertexPtr[m_VertexPtrIndex].Position = transform * m_VertexPositions[i];
			m_VertexPtr[m_VertexPtrIndex].Color = color;
			m_VertexPtr[m_VertexPtrIndex].TextureCoordinates = textureCoordinates[i];
			m_VertexPtr[m_VertexPtrIndex].TextureIndex = texIndex;

			m_VertexPtrIndex++;
		}

		m_IndexCount += 6;
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

		if (!HasRoom()) 
		{
			// Begin new batch
			EndScene();
			Clear();
		}

		float texIndex = 0.0f;

		for (size_t i = 1; i < m_TextureIndex; i++)
		{
			if (m_Textures[i] == font->GetTexutre())
			{
				texIndex = i;
				break;
			}
		}

		if (texIndex == 0.0f)
		{
			texIndex = m_TextureIndex;
			m_Textures[m_TextureIndex] = font->GetTexutre();
			m_TextureIndex++;
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
					m_VertexPtr[m_VertexPtrIndex].Position = transform * vertexPositions[i];
					m_VertexPtr[m_VertexPtrIndex].Color = color;
					m_VertexPtr[m_VertexPtrIndex].TextureCoordinates = uvs[i];
					m_VertexPtr[m_VertexPtrIndex].TextureIndex = texIndex;

					m_VertexPtrIndex++;
				}

				m_IndexCount += 6;

				position.x += glyph->advance_x / 80.0f;
			}
		}
	}

	void BatchRenderer2D::EndScene()
	{
		if (m_IndexCount == 0)
			return;
		
		m_Stats.VertexCount += m_VertexPtrIndex;
		m_Stats.IndexCount +=  m_IndexCount;
		m_Shader->Bind();

		m_VertexArray->Bind();
		m_VertexBuffer->SetData(m_VertexPtr, m_VertexPtrIndex * sizeof(Vertex));

		m_IndexBuffer->Bind();


		for (size_t i = 0; i < m_TextureIndex; i++)
			m_Textures[i]->Bind(i);


		RenderCommand::Draw(*m_VertexArray, m_IndexCount);

		m_Stats.DrawCalls++;

	}

	void BatchRenderer2D::RenderToFramebuffer()
	{
		if (m_Framebuffer->IsSwapChainTarget())
		{
			m_Shader->Unbind();

			m_Framebuffer->BindColorAttachment();

			m_FramebufferShader->Bind();

			m_FramebufferVBO->Bind();
			m_FramebufferIBO->Bind();

			RenderCommand::Draw(*m_FramebufferVAO, 6);

			m_FramebufferShader->Unbind();
			m_Framebuffer->UnbindColorAttachment();

			m_FramebufferVAO->Unbind();
			m_FramebufferVBO->Unbind();
			m_FramebufferIBO->Unbind();

			m_Shader->Bind();
		}
	}

	void BatchRenderer2D::Clear()
	{
		m_VertexPtrIndex = 0;
		m_IndexCount = 0;
		m_TextureIndex = 1;
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
