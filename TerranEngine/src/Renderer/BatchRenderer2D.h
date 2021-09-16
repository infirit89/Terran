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
		Shared<Framebuffer> GetFramebuffer() { return m_Framebuffer; }

		void BeginScene(Camera& camera, const glm::mat4& transform);

		void AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture);
		void AddQuad(glm::mat4& transform, const glm::vec4& color);
		void AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture, glm::vec2 textureCoordinates[4]);

		void AddText(glm::mat4& transform, const glm::vec4& color, Shared<Font> font, const std::string& text);
		void EndScene();

		bool HasRoom() { return !(m_IndexCount >= m_MaxIndices) && !(m_TextureIndex >= m_MaxTextureSlots); }

		void RenderToFramebuffer();

		void ResetStats() { memset(&m_Stats, 0, sizeof(BatchRendererStats)); }
		BatchRendererStats GetStats() { return m_Stats; }

		void EnableObjectCulling(bool state) { m_CullObjectsOutsideOfCamera = state; }
		bool ObjectCulling() { return m_CullObjectsOutsideOfCamera; }

	private:
		void Init(uint32_t batchSize);

		void Close();
		void Clear();

		bool InCameraView(glm::mat4& transform);
		bool InCameraViewX(float x, float width);
		bool InCameraViewY(float y, float height);
		bool InCameraViewZ(float z, float depth);

		static BatchRenderer2D* Get() { return m_Instance; }

	private:
		static BatchRenderer2D* m_Instance;

		BatchRendererStats m_Stats;

		glm::vec4 m_VertexPositions[4];

		uint32_t m_MaxVertices, m_MaxIndices;

		static const uint32_t m_MaxTextureSlots = 16;

		uint32_t m_IndexCount = 0;

		Shared<VertexArray>  m_VertexArray, m_FramebufferVAO;
		Shared<VertexBuffer> m_VertexBuffer, m_FramebufferVBO;
		Shared<IndexBuffer>  m_IndexBuffer, m_FramebufferIBO;

		Shared<Shader> m_Shader, m_FramebufferShader;

		Vertex* m_VertexPtr = nullptr;

		Shared<Texture> m_Textures[m_MaxTextureSlots];

		Shared<Framebuffer> m_Framebuffer;

		struct CameraData
		{
			glm::mat4 Projection;
			glm::mat4 View;

			glm::vec3 ProjectionSize, CameraPosition;
		};

		CameraData m_CameraData;

		Shared<UniformBuffer> m_CameraBuffer;

		uint32_t m_VertexPtrIndex = 0;
		uint32_t m_TextureIndex = 1;
		bool m_CullObjectsOutsideOfCamera = true;
	};
}
