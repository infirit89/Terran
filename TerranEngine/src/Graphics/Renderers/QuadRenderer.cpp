#include "trpch.h"
#include "QuadRenderer.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	QuadRenderer2D::QuadRenderer2D(Shared<IndexBuffer> indexBuffer, glm::vec4* vertexPosition, const Renderer2DStats& stats)
	{
		Init(indexBuffer, vertexPosition, stats);
	}

	QuadRenderer2D::~QuadRenderer2D()
	{
		Destroy();
	}

	void QuadRenderer2D::BeginScene()
	{
		Clear();
	}

	void QuadRenderer2D::EndScene()
	{
		if (m_IndexCount) 
		{
			m_VertexBuffer->SetData(m_VertexPtr, m_VertPtrIndex * sizeof(Vertex));

			for (size_t i = 0; i < m_TexIndex; i++)
				m_Textures[i]->Bind(i);

			RenderCommand::Draw(RenderMode::Triangles, m_VertexArray, m_IndexCount);
		}
	}

	void QuadRenderer2D::Add(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture)
	{

		glm::vec2 texCoords[4] = 
		{
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		float texIndex = -1.0f;

		if (texture == nullptr)
			texIndex = 0.0f;

		for (size_t i = 1; i < m_TexIndex; i++)
		{
			if (m_Textures[i] == texture)
			{
				texIndex = i;
				break;
			}
		}

		if (texIndex == -1.0f) 
		{
			texIndex = m_TexIndex;
			m_Textures[m_TexIndex] = texture;
			m_TexIndex++;
		}

		for (size_t i = 0; i < 4; i++)
		{
			m_VertexPtr[m_VertPtrIndex].Position = transform * m_VertexPositions[i];
			m_VertexPtr[m_VertPtrIndex].TextureIndex = texIndex;
			m_VertexPtr[m_VertPtrIndex].Color = color;
			m_VertexPtr[m_VertPtrIndex].TextureCoordinates = texCoords[i];

			m_VertPtrIndex++;
		}

		m_IndexCount += 6;
	}

	void QuadRenderer2D::Add(glm::mat4& transform, const glm::vec4& color)
	{
		Add(transform, color, nullptr);
	}

	void QuadRenderer2D::Init(Shared<IndexBuffer> indexBuffer, glm::vec4* vertexPosition, const Renderer2DStats& stats)
	{
		m_MaxIndices = stats.MaxIndices;

		m_VertexPositions = vertexPosition;

		m_VertexPtr = new Vertex[stats.MaxVertices];
		m_VertexArray = CreateShared<VertexArray>();

		m_VertexArray->AddVertexBufferLayout({
			{ GL_FLOAT, 3 },
			{ GL_FLOAT, 1 },
			{ GL_FLOAT, 4 },
			{ GL_FLOAT, 2 }
		});

		m_VertexBuffer = CreateShared<VertexBuffer>(stats.MaxVertices * sizeof(Vertex));

		m_VertexArray->AddIndexBuffer(indexBuffer);

		int sampler[16];

		for (size_t i = 0; i < 16; i++)
			sampler[i] = i;

		m_Shader = CreateShared<Shader>("DefaultQuadShader", "res/shaders/Base/QuadVertex.glsl", "res/shaders/Base/QuadFragment.glsl");

		m_Shader->UploadIntArray("u_Sampler", 16, sampler);

		TextureParameters params;

		uint16_t whiteTexureData = 0xffffffff;

		m_Textures[0] = CreateShared<Texture>(1, 1, params);
		m_Textures[0]->SetData(&whiteTexureData);
	}

	void QuadRenderer2D::Destroy()
	{
		delete[] m_VertexPtr;
	}

	void QuadRenderer2D::Clear()
	{
		m_VertPtrIndex = 0;
		m_IndexCount = 0;
		m_TexIndex = 1;

	}
}

