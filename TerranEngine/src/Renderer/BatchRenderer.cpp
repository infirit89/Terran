#include "trpch.h"
#include "BatchRenderer.h"

#include <glm/vec3.hpp>

#include <glad/glad.h>

namespace TerranEngine 
{
	std::vector<BatchData> BatchRenderer::m_Batches;
	uint32_t BatchRenderer::m_BatchSize;
	//RendererStatistics BatchRenderer::m_Stats;

	Shader* quadShader; 
	Shader* textShader;
	void BatchRenderer::Init(uint32_t batchSize)
	{
		m_BatchSize = batchSize;
		m_Batches.reserve(1);

		quadShader = new Shader("res/shaders/default/quad/VertexShader.glsl", "res/shaders/default/quad/FragmentShader.glsl");
		textShader = new Shader("res/shaders/default/quad/VertexShader.glsl", "res/shaders/default/quad/FragmentShader.glsl");
	}

	void BatchRenderer::Close()
	{
		for (BatchData& data : m_Batches)
			Batch::CloseData(data);

		delete quadShader;
		delete textShader;
	}

	void BatchRenderer::AddQuad(glm::mat4& transform, const glm::vec4& color, Texture* texture, glm::vec2 textureCoordinates[4])
	{
		bool added = false;
		for (BatchData& data : m_Batches)
		{
			if (Batch::HasRoom(data) && data.Shader == quadShader)
			{
				Batch::AddQuad(data, transform, color, texture, textureCoordinates);
				added = true;
				break;
			}
		}

		if (!added) {

			BatchData data = Batch::InitData(m_BatchSize, quadShader);
			Batch::AddQuad(data, transform, color, texture, textureCoordinates);
			m_Batches.emplace_back(data);
			//std::sort(m_Batches.begin(), m_Batches.end(), [](BatchData data1, BatchData data2) 
			//{
			//		return data1.Shader != textShader;
			//});
		}
	}

	void BatchRenderer::AddText(glm::mat4& transform, const glm::vec4& color, Font* font, const std::string& text)
	{
		bool added = false;

		for (BatchData& data : m_Batches)
		{
			if (Batch::HasRoom(data) && data.Shader == textShader)
			{
				Batch::AddText(data, transform, color, font, text);
				added = true;
				break;
			}
		}

		if (!added) {
			BatchData data = Batch::InitData(m_BatchSize, textShader);
			Batch::AddText(data, transform, color, font, text);
			m_Batches.emplace_back(data);
			//std::sort(m_Batches.begin(), m_Batches.end(), [](BatchData data1, BatchData data2)
			//{
			//		return data1.Shader != textShader;
			//});
		}
	}

	void BatchRenderer::AddQuad(glm::mat4& transform, const glm::vec4& color, Texture* texture) 
	{
		glm::vec2 textureCoords[4] =
		{
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		AddQuad(transform, color, texture, textureCoords);
	}

	void BatchRenderer::AddQuad(glm::mat4& transform, const glm::vec4& color) 
	{
		AddQuad(transform, color, nullptr);
	}

	void BatchRenderer::Begin()
	{
		for (BatchData& data : m_Batches)
		{
			Batch::Clear(data);
			Batch::ResetStats(data);
		}
	}

	void BatchRenderer::EndScene(Camera& camera, const glm::mat4& transform)
	{
		for (BatchData& data : m_Batches) 
		{
			Batch::BeginScene(data, camera, transform);
			Batch::EndScene(data);
		}
	}

	RendererStats BatchRenderer::GetStats() 
	{
		RendererStats stats;
		stats.Batches = m_Batches.size();

		for (BatchData& data : m_Batches)
			stats.BatchStats.emplace_back(data.BatchStats);

		return stats;
	}
}
