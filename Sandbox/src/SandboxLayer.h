#pragma once

#include "Terran.h"

#include <memory>
#include "Transform.h"

namespace TerranEngine {

	class SandboxLayer : public Layer
	{
	public: 
		SandboxLayer();

		void OnAttach() override;
		void OnDettach() override;

		void Update(float& time) override;

		void OnEvent(Event& event) override;
		void ImGuiRender() override;

		bool KeyPressed(KeyPressedEvent& event);
		bool OnWindowResize(WindowResizeEvent& event);
		bool OnMouseScroll(MouseScrollEvent& event);

	private:
		OrthographicCamera m_Camera;
		Transform m_CameraTransform;
		
		Transform m_Transform1, m_Transform2;

		glm::vec2 m_TexCoords[4];
		
		float m_ZoomLevel = 0.1f;
		
		Shared<Font> m_Font;

		float fps = 0.0f;

		BatchRendererStats stats;

		Unique<BatchRenderer2D> m_Renderer;
		Shared<Scene> m_Scene;

		glm::vec2 m_ViewportSize = {1080.0f, 790.0f};

		glm::uvec2 m_Max = { 10.0f, 10.0f };

		glm::vec4 m_TextColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		float m_Time = 0.0f;

		bool m_Wireframe = false;

		Entity testEntity, cameraEntity;

		SceneSerializer m_SSerializer;
	};
}