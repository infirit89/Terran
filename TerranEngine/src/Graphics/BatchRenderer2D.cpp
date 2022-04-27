#include "trpch.h"
#include "BatchRenderer2D.h"

#include "RenderCommand.h"

#include "Math/Math.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

#pragma warning (push)
#pragma warning (disable : 4305)
#pragma warning (disable : 4309)

namespace TerranEngine 
{
	BatchRenderer2D* BatchRenderer2D::m_Instance;

	BatchRenderer2D::BatchRenderer2D(uint32_t batchSize)
	{
		m_Instance = this;
		Initialize(batchSize);
	}

	BatchRenderer2D::~BatchRenderer2D()
	{
		Shutdown();
	}

	void BatchRenderer2D::Initialize(uint32_t batchSize)
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

		uint16_t whiteTextureData = 0xffffffff;

		int samplers[16];
		for (size_t i = 0; i < m_MaxTextureSlots; i++)
			samplers[i] = i;

		// ******** Quad ******** 
		{
			m_QuadVertexPtr = new QuadVertex[m_MaxVertices];

			m_QuadVAO = CreateShared<VertexArray>();

			m_QuadVBO = CreateShared<VertexBuffer>(m_MaxVertices * sizeof(QuadVertex));

			m_QuadVAO->AddVertexBufferLayout({
				{ GL_FLOAT, 3 },
				{ GL_FLOAT, 4 },
				{ GL_FLOAT, 2 },
				{ GL_INT,	1 }
			});

			m_QuadVAO->AddIndexBuffer(m_IndexBuffer);

			m_QuadShader = CreateShared<Shader>("DefaultQuadShader", 
												"Resources/Shaders/Base/Quad/QuadVertex.glsl", 
												"Resources/Shaders/Base/Quad/QuadFragment.glsl");

			m_QuadShader->UploadIntArray("u_Samplers", m_MaxTextureSlots, samplers);
			m_QuadShader->Unbind();

			m_QuadTextures[0] =	CreateShared<Texture>(1, 1);
			m_QuadTextures[0]->SetData(&whiteTextureData);

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

			m_CircleShader = CreateShared<Shader>(	"DefaultCircleShader", 
													"Resources/Shaders/Base/Circle/CircleVertex.glsl",
													"Resources/Shaders/Base/Circle/CircleFragment.glsl");
			m_CircleShader->Unbind();
		}
		// ************************

		// ******** Line ******** 
		{
			m_LineVertexPtr = new LineVertex[m_MaxVertices];
			m_LineVAO = CreateShared<VertexArray>();
			m_LineVBO = CreateShared<VertexBuffer>(m_MaxVertices * sizeof(LineVertex));

			m_LineVAO->AddVertexBufferLayout({
				{ GL_FLOAT, 3 },
				{ GL_FLOAT, 1 },
				{ GL_FLOAT, 4 },
				{ GL_FLOAT, 2 }
			});

			m_LineVAO->AddIndexBuffer(m_IndexBuffer);

			m_LineShader = CreateShared<Shader>("DefaultLineShader", "Resources/Shaders/Base/Line/LineVertex.glsl", "Resources/Shaders/Base/Line/LineFragment.glsl");
			m_LineShader->Unbind();
		}
		// **********************

		m_VertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		m_VertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		m_VertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		m_VertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		m_QuadShader->Bind();
		m_CircleShader->Bind();
		m_LineShader->Bind();

		m_CameraBuffer = CreateShared<UniformBuffer>(sizeof(CameraData), 0);

		m_QuadShader->Unbind();
		m_CircleShader->Unbind();
		m_LineShader->Unbind();
	}

	void BatchRenderer2D::Shutdown()
	{
		delete[] m_QuadVertexPtr;
		delete[] m_CircleVertexPtr;
		delete[] m_LineVertexPtr;
	}

	void BatchRenderer2D::BeginFrame(Camera& camera, const glm::mat4& transform, bool inverseView)
	{
		Clear();

		m_QuadShader->Bind();
		m_CircleShader->Bind();
		m_LineShader->Bind();

		m_CameraData.Projection = camera.GetProjection();
		m_CameraData.View = transform;

		if (inverseView)
			m_CameraData.View = glm::inverse(m_CameraData.View);

		m_CameraBuffer->Bind();
		m_CameraBuffer->SetData(&m_CameraData, 0, sizeof(CameraData));

		m_QuadShader->Unbind();
		m_CircleShader->Unbind();
		m_LineShader->Unbind();
	}

	void BatchRenderer2D::AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture)
	{
		glm::vec2 textureCoords[4] = 
		{
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
		};

		AddQuad(transform, color, texture, textureCoords);
	}

	void BatchRenderer2D::AddQuad(glm::mat4& transform, const glm::vec4& color)
	{
		AddQuad(transform, color, nullptr);
	}

	void BatchRenderer2D::AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture, glm::vec2 textureCoordinates[4])
	{
		if (!QuadBatchHasRoom()) 
		{
			// Begin New Batch
			EndFrame();
			Clear();
		}

		int texIndex = -1;

		if (texture == nullptr)
			texIndex = 0;

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

	void BatchRenderer2D::AddCircle(glm::mat4& transform, const glm::vec4& color, float thickness)
	{
		if (!CircleBatchHasRoom())
		{
			// Begin New Batch
			EndFrame();
			Clear();
		}

		for (size_t i = 0; i < 4; i++)
		{
			m_CircleVertexPtr[m_CircleVertexPtrIndex].Position = transform * m_VertexPositions[i];
			m_CircleVertexPtr[m_CircleVertexPtrIndex].Thickness = thickness;
			m_CircleVertexPtr[m_CircleVertexPtrIndex].Color = color;
			m_CircleVertexPtr[m_CircleVertexPtrIndex].LocalPosition = glm::vec2(m_VertexPositions[i].x, m_VertexPositions[i].y) * 2.0f;

			m_CircleVertexPtrIndex++;
		}

		m_CircleIndexCount += 6;
	}

	void BatchRenderer2D::AddLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec4& color, float thickness)
	{
		if (!LineBatchHasRoom()) 
		{
			EndFrame();
			Clear();
		}

		const glm::vec3 points[2] = { point1, point2 };

		glm::vec3 lineNormal = glm::cross(point1, point2);

		m_LineVertexPtr[m_LineVertexPtrIndex].Position = points[0];
		m_LineVertexPtr[m_LineVertexPtrIndex].Thickness = thickness;
		m_LineVertexPtr[m_LineVertexPtrIndex].Color = color;
		m_LineVertexPtr[m_LineVertexPtrIndex].Normal = -glm::normalize(lineNormal);

		m_LineVertexPtrIndex++;

		m_LineVertexPtr[m_LineVertexPtrIndex].Position = points[0];
		m_LineVertexPtr[m_LineVertexPtrIndex].Thickness = thickness;
		m_LineVertexPtr[m_LineVertexPtrIndex].Color = color;

		m_LineVertexPtr[m_LineVertexPtrIndex].Normal = glm::normalize(lineNormal);

		m_LineVertexPtrIndex++;

		m_LineVertexPtr[m_LineVertexPtrIndex].Position = points[1];
		m_LineVertexPtr[m_LineVertexPtrIndex].Thickness = thickness;
		m_LineVertexPtr[m_LineVertexPtrIndex].Color = color;

		m_LineVertexPtr[m_LineVertexPtrIndex].Normal = glm::normalize(lineNormal);

		m_LineVertexPtrIndex++;

		m_LineVertexPtr[m_LineVertexPtrIndex].Position = points[1];
		m_LineVertexPtr[m_LineVertexPtrIndex].Thickness = thickness;
		m_LineVertexPtr[m_LineVertexPtrIndex].Color = color;

		m_LineVertexPtr[m_LineVertexPtrIndex].Normal = -glm::normalize(lineNormal);

		m_LineVertexPtrIndex++;


		m_LineIndexCount += 6;
	}

	void BatchRenderer2D::AddLine(const glm::vec3 points[], int pointCount, const glm::vec4& color, float thickness)
	{
		int timesToAdd = pointCount / 2;

		for (size_t i = 0; i < timesToAdd; i++) 
		{
			int ind = i + 1 * i;
			AddLine(points[ind], points[ind + 1], color, thickness);
		}
	}

	void BatchRenderer2D::AddRect(const glm::mat4& transform, const glm::vec4& color, float thickness)
	{
		glm::vec3 linePositions[4];

		for (size_t i = 0; i < 4; i++)
			linePositions[i] = transform * m_VertexPositions[i];

		AddLine(linePositions[0], linePositions[1], color, thickness);
		AddLine(linePositions[1], linePositions[2], color, thickness);
		AddLine(linePositions[2], linePositions[3], color, thickness);
		AddLine(linePositions[3], linePositions[0], color, thickness);
	}

	void BatchRenderer2D::AddRect(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, float thickness)
	{
		AddLine({ position.x - size.x * 0.5f, position.y + size.y * 0.5f, 1.0f }, { position.x - size.x * 0.5f, position.y - size.y * 0.5f, 1.0f }, color, thickness);
		AddLine({ position.x - size.x * 0.5f, position.y + size.y * 0.5f, 1.0f }, { position.x + size.x * 0.5f, position.y + size.y * 0.5f, 1.0f }, color, thickness);
		AddLine({ position.x + size.x * 0.5f, position.y + size.y * 0.5f, 1.0f }, { position.x + size.x * 0.5f, position.y - size.y * 0.5f, 1.0f }, color, thickness);
		AddLine({ position.x - size.x * 0.5f, position.y - size.y * 0.5f, 1.0f }, { position.x + size.x * 0.5f, position.y - size.y * 0.5f, 1.0f }, color, thickness);
	}

	void BatchRenderer2D::EndFrame()
	{
		m_Stats.VertexCount += m_QuadVertexPtrIndex + m_CircleVertexPtrIndex + m_LineVertexPtrIndex;
		m_Stats.IndexCount +=  m_QuadIndexCount + m_CircleIndexCount + m_LineIndexCount;

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

		// Submit lines
		if (m_LineIndexCount) 
		{
			m_LineShader->Bind();
			m_LineVAO->Bind();
			m_LineVBO->SetData(m_LineVertexPtr, m_LineVertexPtrIndex * sizeof(LineVertex));

			RenderCommand::Draw(RenderMode::Triangles, m_LineVAO, m_LineIndexCount);

			m_Stats.DrawCalls++;

			m_LineShader->Unbind();
		}
	}

	void BatchRenderer2D::Clear()
	{
		m_QuadVertexPtrIndex = 0;
		m_QuadIndexCount = 0;
		m_QuadTextureIndex = 1;

		m_CircleVertexPtrIndex = 0;
		m_CircleIndexCount = 0;

		m_LineVertexPtrIndex = 0;
		m_LineIndexCount = 0;
	}
}
#pragma warning (pop)
