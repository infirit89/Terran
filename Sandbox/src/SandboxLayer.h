#pragma once

#include "Terran.h"

#include <memory>

namespace TerranEngine {

	class SandboxLayer : public Layer
	{
	public: 
		SandboxLayer();

		void Update(float& time) override;

		void OnEvent(Event& event) override;
		void ImGuiRender() override;

		bool KeyPressed(KeyPressedEvent& event);
	private:
		std::shared_ptr<OrthographicCamera> m_Camera;
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<Texture> m_Texture;
	};
}