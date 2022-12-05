#pragma once

#include "Core/Base.h"

#include "Camera.h"
#include "FontAtlas.h"

#include <glm/glm.hpp>

namespace TerranEngine 
{
	struct BatchRendererStats
	{
		uint32_t MaxVertices = 0;
		uint32_t MaxIndices = 0;
		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0;

		uint32_t DrawCalls = 0;
	};

	// TODO: make it static; because every other system is static
	class BatchRenderer2D
	{
	public:
		static void Initialize(uint32_t batchSize);
		static void Shutdown();

		static void BeginFrame(Camera& camera, const glm::mat4& transform, bool inverseView);
		static void EndFrame();
		
		static void AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture, int entityID = -1);
		static void AddQuad(glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture, glm::vec2 textureCoordinates[4], int entityID = -1);
		
		static void AddCircle(glm::mat4& transform, const glm::vec4& color, float thickness, int entityID = -1);
		
		static void AddLine(const glm::vec3& startPoint, const glm::vec3& endPoint, const glm::vec4& color, float thickness, int entityID = -1);
		
		static void AddDebugLine(const glm::vec3& startPoint, const glm::vec3& endPoint, const glm::vec4& color);
		static void AddDebugRect(const glm::mat4& transform, const glm::vec4& color);
		
		// TODO: use wide string
		static void AddText(glm::mat4& transform, const std::string& text, const glm::vec4& color, Shared<FontAtlas> fontAtlas, 
			float lineSpacing = 1.0f, float lineWidth = 10.0f, int entityID = -1);

		static void ResetStats();
		static BatchRendererStats GetStats();
	private:
		static void Clear();

		static bool QuadBatchHasRoom();
		static bool CircleBatchHasRoom();
		static bool LineBatchHasRoom();
		static bool TextBatchHasRoom();

	};
}
