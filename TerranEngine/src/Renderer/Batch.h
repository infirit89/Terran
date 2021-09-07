#pragma once

#include <glm/glm.hpp>

#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Font.h"
#include "UniformBuffer.h"

//#include "BatchRenderer.h"

#include "Scene/Camera.h"

#include <memory>

#include <freetype-gl.h>

namespace TerranEngine 
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoordinates;
		float TextureIndex;
	};

	struct BatchStats
	{
		uint32_t MaxVertices = 0;
		uint32_t MaxIndices = 0;
		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0;

		uint32_t GetQuadCount() { return VertexCount / 4; }
	};

	struct BatchData 
	{
		uint32_t MaxVertices, MaxIndices;
		static const uint32_t MaxTextureSlots = 16;

		uint32_t IndexCount = 0;

		VertexArray* VertexArray;
		VertexBuffer* VertexBuffer;
		Shader* Shader;

		Vertex* VertexPtr;

		Texture* Textures[MaxTextureSlots];

		struct CameraData
		{
			glm::mat4 projection;
			glm::mat4 view;
		};

		CameraData CameraData;

		UniformBuffer* CameraBuffer;

		uint32_t VertexPtrIndex = 0;
		uint32_t TextureIndex = 1;

		BatchStats BatchStats;
	};

	class Batch 
	{
	public:

		static BatchData InitData(uint32_t batchSize, Shader* shader);
		static void CloseData(BatchData& data);

		static void BeginScene(BatchData& data, Camera& camera, const glm::mat4& transform);
		static void AddQuad(BatchData& data, glm::mat4& transform, const glm::vec4& color, Texture* texture, glm::vec2 textureCoordinates[4]);
		static void AddText(BatchData& data, glm::mat4& transform, const glm::vec4& color, Font* font, const std::string& text);
		static void EndScene(BatchData& data);

		static void Clear(BatchData& data);

		static bool HasRoom(BatchData& data) { return !(data.IndexCount >= data.MaxIndices) && !(data.TextureIndex >= data.MaxTextureSlots); }

		static void ResetStats(BatchData& data);

	private:
		static glm::vec4 s_VertexPositions[4];

		static int* s_Indices;
		static IndexBuffer* s_IndexBuffer;
	};
}
