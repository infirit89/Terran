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
	private:
		OrthographicCamera m_Camera;
		glm::mat4 m_ViewMatrix;
		Transform m_Transform;
		/*std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<Texture> m_Texture;*/

		Shader* m_Shader;
		VertexBuffer* m_VertexBuffer;
		IndexBuffer* m_IndexBuffer;
		VertexArray* m_VertexArray;

		Texture* m_Texture;
	};
}