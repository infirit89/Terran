#pragma once

#include <glm/glm.hpp>

#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Font.h"
#include "UniformBuffer.h"
#include "Framebuffer.h"

#include "Camera.h"
#include "Core/Base.h"

namespace TerranEngine 
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoordinates;
		float TextureIndex;
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

		void CreateFramebuffer(uint32_t width, uint32_t height, bool swapchainTarget);
		inline Shared<Framebuffer> GetFramebuffer() { return m_Framebuffer; }

		void BeginScene(Camera& camera, const glm::mat4& transform);
		void EndScene();

		void AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture);
		void AddQuad(glm::mat4& transform, const glm::vec4& color);
		void AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture, glm::vec2 textureCoordinates[4]);

		void AddText(glm::mat4& transform, const glm::vec4& color, Shared<Font> font, const std::string& text);

		inline bool QuadBatchHasRoom() { return !(m_QuadIndexCount >= m_MaxIndices) && !(m_QuadTextureIndex >= m_MaxTextureSlots); }
		inline bool TextBatchHasRoom() { return !(m_TextIndexCount >= m_MaxIndices) && !(m_TextTextureIndex >= m_MaxTextureSlots); }

		void RenderToFramebuffer();

		inline void ResetStats() { memset(&m_Stats, 0, sizeof(BatchRendererStats)); }
		inline BatchRendererStats GetStats() { return m_Stats; }

		inline void EnableObjectCulling(bool state) { m_CullObjectsOutsideOfCamera = state; }
		inline bool ObjectCulling() { return m_CullObjectsOutsideOfCamera; }

		static BatchRenderer2D* Get() { return m_Instance; }
	private:

		void Init(uint32_t batchSize);

		void Close();
		void Clear();

		bool InCameraView(glm::mat4& transform);
		bool InCameraViewX(float x, float width);
		bool InCameraViewY(float y, float height);
		bool InCameraViewZ(float z, float depth);


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

		Vertex* m_QuadVertexPtr = nullptr;
		uint32_t m_QuadVertexPtrIndex = 0;

		Shared<Texture> m_QuadTextures[m_MaxTextureSlots];
		uint32_t m_QuadTextureIndex = 1;
		// **********************


		// ******** Text ********
		uint32_t m_TextIndexCount = 0;
		Shared<VertexArray>  m_TextVAO;
		Shared<VertexBuffer> m_TextVBO;

		Shared<Shader> m_TextShader;

		Vertex* m_TextVertexPtr = nullptr;
		uint32_t m_TextVertexPtrIndex = 0;

		Shared<Texture> m_TextTextures[m_MaxTextureSlots];
		uint32_t m_TextTextureIndex = 1;
		// **********************


		// ******** Frame buffer ********
		Shared<VertexArray> m_FramebufferVAO;
		Shared<VertexBuffer> m_FramebufferVBO;
		Shared<IndexBuffer> m_FramebufferIBO;
		Shared<Shader> m_FramebufferShader;

		Shared<Framebuffer> m_Framebuffer;
		// ******************************


		//  ******** Camera stuffs ********
		struct CameraData
		{
			glm::mat4 Projection;
			glm::mat4 View;

			glm::vec3 ProjectionSize, CameraPosition;
		};

		CameraData m_CameraData;

		Shared<UniformBuffer> m_CameraBuffer;

		bool m_CullObjectsOutsideOfCamera = false;
		// ********************************
	};
}
