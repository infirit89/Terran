#include "SandboxLayer.h"

#include <imgui.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace TerranEngine 
{
	SandboxLayer::SandboxLayer()
		: Layer("Sandbox Layer")
	{
		float positions[] =
		{
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		int indices[]
		{
			0, 1, 2,
			2, 3, 0
		};

		m_VertexArray = std::make_shared<VertexArray>();
		m_VertexBuffer.reset(new VertexBuffer(positions, sizeof(positions)));

		m_VertexBuffer->SetLayout({
			{ BufferElementType::Type::FLOAT3 },
			{ BufferElementType::Type::FLOAT2 }
		});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		m_IndexBuffer.reset(new IndexBuffer(indices, sizeof(positions)));

		m_Shader.reset(new Shader("res/VertexShader.shader", "res/FragmentShader.shader"));
		m_Shader->Bind();

		m_Texture.reset(new Texture("res/test_grass.png"));
		m_Texture->Bind(0);

		m_Camera.reset(new OrthographicCamera(Application::Get()->GetWindow().GetWidth() * 0.01f, Application::Get()->GetWindow().GetHeight() * 0.01f));
		glm::mat4 viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

		m_Shader->UploadMat4("u_ProjMat", m_Camera->ProjectionMat);
		m_Shader->UploadMat4("u_ViewMat", viewMat);
	}

	void SandboxLayer::Update(float& time)
	{
		RendererCommand::Draw(m_VertexArray, 6);
	}

	void SandboxLayer::OnEvent(Event& event)
	{
		EventDispatcher dispather(event);
		dispather.Dispatch<KeyPressedEvent>(TR_EVENT_BIND_FN(SandboxLayer::KeyPressed));
	}

	void SandboxLayer::ImGuiRender()
	{
		ImGui::ShowDemoWindow();
	}

	bool SandboxLayer::KeyPressed(KeyPressedEvent& event)
	{
		if (event.GetRepeatCount() > 0)
			return false;

		bool isCtrlPressed = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);

		switch (event.GetKeyCode())
		{
			case Key::X: 
			{
				if (isCtrlPressed)
					Application::Get()->Close();
				break;
			}
		}

		return false;
	}
}

