#pragma once

#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "UniformBuffer.h"
#include "Framebuffer.h"
#include "Camera.h"

#include "FontAtlas.h"

#include "Core/Base.h"

#include <glm/glm.hpp>

namespace TerranEngine 
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoordinates;
		int TextureIndex;
		int EntityID;
	};

	struct CircleVertex 
	{
		glm::vec3 Position;
		float Thickness;
		glm::vec4 Color;
		glm::vec2 LocalPosition;
	};

	struct LineVertex 
	{
		glm::vec3 Position;
		float Thickness;
		glm::vec4 Color;
		glm::vec2 Normal;
	};
	
	struct TextVertex 
	{
		glm::vec3 Position;
		int TextureIndex;
		glm::vec4 TextColor;
		glm::vec2 TextureCoordinates;
	};

	struct BatchRendererStats
	{
		uint32_t MaxVertices = 0;
		uint32_t MaxIndices = 0;
		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0;

		uint32_t DrawCalls = 0;

		uint32_t GetQuadCount() { return VertexCount / 4; }
	};

	class BatchRenderer2D
	{
	public:
		BatchRenderer2D(uint32_t batchSize);

		~BatchRenderer2D();

		void BeginFrame(Camera& camera, const glm::mat4& transform, bool inverseView);
		void EndFrame();

		void AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture, int entityID = -1);
		void AddQuad(glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		void AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture, glm::vec2 textureCoordinates[4], int entityID = -1);

		void AddCircle(glm::mat4& transform, const glm::vec4& color, float thickness);

		void AddLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec4& color, float thickness);
		void AddLine(const glm::vec3 points[], int pointCount, const glm::vec4& color, float thickness);

		// TODO: use wide string
		void AddText(glm::mat4& transform, const std::string& text, const glm::vec4& color, Shared<FontAtlas> fontAtlas, float lineSpacing = 1.0f, float lineWidth = 10.0f);
		
		void AddRect(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, float thickness);
		void AddRect(const glm::mat4& transform, const glm::vec4& color, float thickness);

		inline bool QuadBatchHasRoom() const { return !(m_QuadIndexCount >= m_MaxIndices) && !(m_QuadTextureIndex >= m_MaxTextureSlots); }
		
		inline bool CircleBatchHasRoom() const { return !(m_CircleIndexCount >= m_MaxIndices); }

		inline bool LineBatchHasRoom() const { return !(m_LineIndexCount >= m_MaxIndices); }
		
		inline bool TextBatchHasRoom() const { return !(m_TextIndexCount >= m_MaxIndices) && !(m_TextTextureIndex >= m_MaxTextureSlots); }

		inline void ResetStats() { memset(&m_Stats, 0, sizeof(BatchRendererStats)); }
		inline BatchRendererStats GetStats() { return m_Stats; }

		static BatchRenderer2D* Get() { return m_Instance; }
	private:
		void Initialize(uint32_t batchSize);
		void Shutdown();

		void Clear();
	private:
		// ******** Base stuffs ********
		static BatchRenderer2D* m_Instance;
		BatchRendererStats m_Stats;
		glm::vec4 m_VertexPositions[4];
		uint32_t m_MaxVertices, m_MaxIndices;

		static const uint32_t m_MaxTextureSlots = 16;
		// *****************************

		Shared<IndexBuffer>  m_IndexBuffer;

		// ******** Quad ********
		uint32_t m_QuadIndexCount = 0;
		Shared<VertexArray>  m_QuadVAO;
		Shared<VertexBuffer> m_QuadVBO;

		Shared<Shader> m_QuadShader;

		QuadVertex* m_QuadVertexPtr = nullptr;
		uint32_t m_QuadVertexPtrIndex = 0;

		Shared<Texture> m_QuadTextures[m_MaxTextureSlots];
		uint32_t m_QuadTextureIndex = 1;
		// **********************

		// ******** Circle ********
		uint32_t m_CircleIndexCount = 0;
		Shared<VertexArray>  m_CircleVAO;
		Shared<VertexBuffer> m_CircleVBO;

		Shared<Shader> m_CircleShader;

		CircleVertex* m_CircleVertexPtr = nullptr;
		uint32_t m_CircleVertexPtrIndex = 0;
		// ************************

		// NOTE: shitty line renderer should change soon
		// ******** Line ******** 
		uint32_t m_LineIndexCount = 0;
		Shared<Shader> m_LineShader;

		Shared<VertexArray> m_LineVAO;
		Shared<VertexBuffer> m_LineVBO;

		LineVertex* m_LineVertexPtr = nullptr;
		uint32_t m_LineVertexPtrIndex = 0;
		// **********************

		// ******** Text ********
		uint32_t m_TextIndexCount = 0;
		Shared<VertexBuffer> m_TextVBO;
		Shared<VertexArray> m_TextVAO;

		Shared<Shader> m_TextShader;

		TextVertex* m_TextVertexPtr = nullptr;
		uint32_t m_TextVertexPtrIndex = 0;

		Shared<Texture> m_TextTextures[m_MaxTextureSlots];
		uint32_t m_TextTextureIndex = 0;

		// **********************

		//  ******** Camera stuffs ********
		struct CameraData
		{
			glm::mat4 Projection;
			glm::mat4 View;
		};

		CameraData m_CameraData;

		Shared<UniformBuffer> m_CameraBuffer;
	};
}
