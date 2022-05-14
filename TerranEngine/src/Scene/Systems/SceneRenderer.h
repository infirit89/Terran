#pragma once

#include "Core/Base.h"

#include "Graphics/Framebuffer.h"
#include "Graphics/Camera.h"
#include "Graphics/Texture.h"

#include "Scene/Components.h"
#include "Scene/Scene.h"

namespace TerranEngine 
{
	class SceneRenderer 
	{
	public:
		SceneRenderer(FramebufferParameters params);

		void SetClearColor(glm::vec4 color) { m_ClearColor = color; }

		void SetScene(Scene* scene);

		void BeginScene(Camera& camera, glm::mat4& cameraTransform, bool invereTransform);

		void SubmitSprite(SpriteRendererComponent& spriteRenderer, glm::mat4& transform, int entityID = -1);
		void SubmitCircle(CircleRendererComponent& circleRenderer, glm::mat4& transform);
		void SubmitLine(LineRendererComponent& lineRenderer);

		void SubmitText(TextRendererComponent& textRenderer, glm::mat4& transform);

		void SubmitDebugRectangle(const glm::mat4& transform, const glm::vec4& color, float thickness);

		uint32_t GetViewportWidth() { return m_Width; }
		uint32_t GetViewportHeight() { return m_Height; }

		void EndScene();

		Shared<Framebuffer>& GetFramebuffer() { return m_Framebuffer; }

		void OnResize(uint32_t width, uint32_t height);

		void SetShowColliders(bool show) { m_ShowColliders = show; }
		bool AreCollidersShowing() { return m_ShowColliders; }

	private:
		void SubmitColliderBounds();

		Scene* m_Scene;
		bool m_BegunScene = false;
		bool m_ShowColliders = false;

		glm::vec4 m_ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		uint32_t m_Width = 1080, m_Height = 720;

		Shared<Framebuffer> m_Framebuffer;
	};
}