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

		void Update(Time& time) override;

		void OnEvent(Event& event) override;
		void ImGuiRender() override;

		bool KeyPressed(KeyPressedEvent& event);
		bool OnWindowResize(WindowResizeEvent& event);
		bool OnMouseScroll(MouseScrollEvent& event);

	private:
		OrthographicCamera m_Camera;
		Transform m_CameraTransform;
		Transform m_Transform;

		float m_ZoomLevel = 0.1f;
		glm::vec2 m_ViewportSize = { 1080.0f, 790.0f };

		Shared<Framebuffer> m_Framebuffer;
	};
}