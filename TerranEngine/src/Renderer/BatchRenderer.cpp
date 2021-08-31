#include "trpch.h"
#include "BatchRenderer.h"

#include <glm/vec3.hpp>

#include <glad/glad.h>

namespace TerranEngine 
{
	std::vector<BatchData> BatchRenderer::m_Batches;
	uint32_t BatchRenderer::m_BatchSize;
	int BatchRenderer::m_TimesAdded;

	Shader* quadShader; 
	Shader* textShader;
	void BatchRenderer::Init(uint32_t batchSize)
	{
		m_BatchSize = batchSize;
		m_Batches.reserve(1);

		quadShader = new Shader("res/shaders/default/quad/VertexShader.glsl", "res/shaders/default/quad/FragmentShader.glsl");
		textShader = new Shader("res/shaders/default/text/TextVertex.glsl", "res/shaders/default/text/TextFragment.glsl");
		m_TimesAdded = 0;
	}

	void BatchRenderer::Close()
	{
		for (BatchData& data : m_Batches)
			Batch::CloseData(data);

		delete quadShader;
		delete textShader;
	}

	void BatchRenderer::AddQuad(glm::mat4& transform, const glm::vec4& color, uint32_t zIndex, Texture* texture, glm::vec2 textureCoordinates[4])
	{
		bool added = false;
		for (BatchData& data : m_Batches)
		{
			if (Batch::HasRoom(data) && data.ZIndex == zIndex && data.Shader == quadShader)
			{
				Batch::AddQuad(data, transform, color, texture, textureCoordinates);
				added = true;
				break;
			}
		}

		if (!added) {

			BatchData data = Batch::InitData(m_BatchSize, zIndex, quadShader);
			Batch::AddQuad(data, transform, color, texture, textureCoordinates);
			m_Batches.emplace_back(data);
			std::sort(m_Batches.begin(), m_Batches.end(), [](BatchData data1, BatchData data2) 
			{
					return data1.ZIndex < data2.ZIndex;
			});
		}
	}

	void BatchRenderer::AddText(glm::mat4& transform, const glm::vec4& color, uint32_t zIndex, Font* font, const std::string& text)
	{
		bool added = false;

		for (BatchData& data : m_Batches)
		{
			if (Batch::HasRoom(data) && data.ZIndex == zIndex && data.Shader == textShader)
			{
				Batch::AddText(data, transform, color, font, text);
				added = true;
				break;
			}
		}

		if (!added) {
			BatchData data = Batch::InitData(m_BatchSize, zIndex, textShader);
			Batch::AddText(data, transform, color, font, text);
			m_Batches.emplace_back(data);
			std::sort(m_Batches.begin(), m_Batches.end(), [](BatchData data1, BatchData data2)
			{
					return data1.ZIndex < data2.ZIndex;
			});
		}
	}

	void BatchRenderer::AddQuad(glm::mat4& transform, const glm::vec4& color, uint32_t zIndex, Texture* texture) 
	{
		glm::vec2 textureCoords[4] =
		{
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		AddQuad(transform, color, zIndex, texture, textureCoords);
	}

	void BatchRenderer::AddQuad(glm::mat4& transform, const glm::vec4& color, uint32_t zIndex) 
	{
		AddQuad(transform, color, zIndex, nullptr);
	}

	void BatchRenderer::EndScene(Camera& camera, const glm::mat4& transform)
	{
		for (BatchData& data : m_Batches) 
		{
			Batch::BeginScene(data, camera, transform);
			Batch::EndScene(data);
			Batch::Clear(data);
		}

		m_TimesAdded = 0;
	}
}
