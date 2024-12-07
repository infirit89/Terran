#pragma once

#include "Core/Base.h"

#include "Graphics/Framebuffer.h"
#include "Graphics/Camera.h"
#include "Graphics/Texture.h"

#include "Scene/Components.h"
#include "Scene/Scene.h"

namespace TerranEngine 
{
	class SceneRenderer final
	{
	public:
		SceneRenderer(const FramebufferParameters& params);

		void SetClearColor(glm::vec4 color) { m_ClearColor = color; }

		void SetScene(Scene* scene);

		void BeginScene(Camera& camera, const glm::mat4& cameraTransform, bool inverseTransform);

		void SubmitSprite(const SpriteRendererComponent& spriteRenderer, glm::mat4& transform, int entityID);
		void SubmitCircle(const CircleRendererComponent& circleRenderer, glm::mat4& transform, int entityID);
		void SubmitLine(const LineRendererComponent& lineRenderer, int entityID);

		void SubmitText(const TextRendererComponent& textRenderer, glm::mat4& transform, int entityID);

		void EndScene();

		Shared<Framebuffer>& GetFramebuffer() { return m_Framebuffer; }

		void OnResize(uint32_t width, uint32_t height);

		void SetShowColliders(bool show) { m_ShowColliders = show; }
		bool AreCollidersShowing() const { return m_ShowColliders; }

	private:
		void SubmitColliderBounds();

		Scene* m_Scene;
		bool m_BegunScene = false;
		bool m_ShowColliders = false;

		glm::vec4 m_ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		Shared<Framebuffer> m_Framebuffer;
	};
}