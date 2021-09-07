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
		/*std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<Texture> m_Texture;*/
		
		TextureSheet m_TSheet;

		float m_ZoomLevel = 0.1f;

		Texture* m_Texture;
		Texture* m_Texture2;

		Font* m_Font;
		float fps = 0.0f;

		RendererStats stats;

		float m_Time = 0.0f;
	};
}