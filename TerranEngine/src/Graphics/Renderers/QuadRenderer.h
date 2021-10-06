#pragma once

#include "Graphics/Buffer.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Renderer2DStats.h"

#include <glm/glm.hpp>

namespace TerranEngine 
{
	struct Vertex 
	{
		glm::vec3 Position;
		float TextureIndex;
		glm::vec4 Color;
		glm::vec2 TextureCoordinates;
	};

	class QuadRenderer2D 
	{
	public:
		QuadRenderer2D(Shared<IndexBuffer> indexBuffer, glm::vec4* vertexPosition, const Renderer2DStats& stats);
		~QuadRenderer2D();

		void BeginScene();
		void EndScene();

		void Add(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture);
		void Add(glm::mat4& transform, const glm::vec4& color);

		bool HasRoom() { return ((m_IndexCount >= m_MaxIndices) && (m_TexIndex >= 16)); }

	private:
		void Init(Shared<IndexBuffer> indexBuffer, glm::vec4* vertexPosition, const Renderer2DStats& stats);
		void Destroy();

		void Clear();

		glm::vec4* m_VertexPositions;

		uint32_t m_MaxIndices;

		Vertex* m_VertexPtr;
		uint32_t m_VertPtrIndex;

		Shared<VertexBuffer> m_VertexBuffer;
		Shared<VertexArray> m_VertexArray;

		Shared<Shader> m_Shader;

		uint32_t m_IndexCount = 0;

		Shared<Texture> m_Textures[16];
		uint32_t m_TexIndex = 1;
	};
}