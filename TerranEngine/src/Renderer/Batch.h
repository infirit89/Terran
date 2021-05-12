#pragma once

#include <glm/glm.hpp>

#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "Scene/Camera.h"

#include <memory>

namespace TerranEngine 
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoordinates;
		float TextureIndex;
	};

	struct BatchData 
	{
		uint32_t MaxVertices, MaxIndices;
		static const uint32_t MaxTextureSlots = 16;

		uint32_t ZIndex = 0;

		uint32_t IndexCount = 0;

		VertexArray* VertexArray;
		VertexBuffer* VertexBuffer;
		IndexBuffer* IndexBuffer;
		Shader* Shader;

		Vertex* VertexPtr;

		Texture* Textures[MaxTextureSlots];

		uint32_t VertexPtrIndex = 0;
		uint32_t TextureIndex = 1;
	};

	class Batch 
	{
	public:

		static BatchData InitData(uint32_t batchSize, uint32_t zIndex);
		static void CloseData(BatchData& data);

		static void BeginScene(BatchData& data, Camera& camera, const glm::mat4& transform);
		static void AddQuad(BatchData& data, const glm::mat4& transform, const glm::vec4& color, Texture* texture, glm::vec2 textureCoordinates[4]);
		static void EndScene(BatchData& data);

		static void Clear(BatchData& data);

		static bool HasRoom(BatchData& data) { return !(data.IndexCount >= data.MaxIndices) && !(data.TextureIndex >= data.MaxTextureSlots); }
	
	private:
		static glm::vec4 s_VertexPositions[4];

		static int* s_Indices;
	};
}