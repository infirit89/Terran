#pragma once

#include "Batch.h"

#include <vector>

namespace TerranEngine 
{
	class BatchRenderer 
	{
	public:
		static void Init(uint32_t batchSize);

		static void Close();

		static void AddQuad(const glm::mat4& transform, const glm::vec4& color, uint32_t zIndex, Texture* texture, glm::vec2 textureCoordnites[4]);
		static void AddQuad(const glm::mat4& transform, const glm::vec4& color, uint32_t zIndex, Texture* texture);
		static void AddQuad(const glm::mat4& transform, const glm::vec4& color, uint32_t zIndex);

		static void EndScene(Camera& camera, const glm::mat4& transform);

	private:
		static std::vector<BatchData> m_Batches;
		static uint32_t m_BatchSize;
	};

	/* 
		old shitty fyuckking batch renderer
		hahahah poop stinky
	*/

	/*struct Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoords;
		float TextureIndex;
	};

	struct Data
	{
		uint32_t MaxVertices, MaxIndices;

		static const uint32_t MaxTextureSlots = 16;

		VertexArray* VertexArray;
		VertexBuffer* VertexBuffer;
		IndexBuffer* IndexBuffer;

		Shader* shader;

		Vertex* VertexArrayPtr;
		uint32_t VertexArrayIndex;

		uint32_t IndexCount = 0;
		uint32_t TexIndex = 1;

		Texture* WhiteTexture;

		Texture* Textures[MaxTextureSlots];

		glm::vec4 VertexPositions[4];
		glm::vec2 TextureCoordinates[4];
	};

	class BatchRenderer 
	{
	public:
		static void Init(uint32_t batchSize);
		static void Close();

		static void BeginScene(Camera& camera, const glm::mat4& transform);
		static void EndScene();

		static void Draw(const glm::mat4& transform, const glm::vec4& color);
		static void Draw(const glm::mat4& transform, const glm::vec4& color, Texture& texture);
	private:
		static void BeginNewBatch();
		static void Submit();
		

		static Data s_Data;
	};*/
}