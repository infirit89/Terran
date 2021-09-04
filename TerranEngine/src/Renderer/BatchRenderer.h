#pragma once

#include "Batch.h"

#include <vector>

namespace TerranEngine 
{
	struct RendererStats 
	{
		std::vector<BatchStats> BatchStats;
		uint32_t Batches = 0;
	};

	class BatchRenderer 
	{
	public:
		static void Init(uint32_t batchSize);

		static void Close();

		static void AddQuad(glm::mat4& transform, const glm::vec4& color, Texture* texture, glm::vec2 textureCoordnites[4]);
		static void AddQuad(glm::mat4& transform, const glm::vec4& color, Texture* texture);
		static void AddQuad(glm::mat4& transform, const glm::vec4& color);

		static void AddText(glm::mat4& transform, const glm::vec4& color, Font* font, const std::string& text);
		
		static void Begin();
		static void EndScene(Camera& camera, const glm::mat4& transform);

		static RendererStats GetStats();

	private:
		static std::vector<BatchData> m_Batches;
		static uint32_t m_BatchSize;
	};

}