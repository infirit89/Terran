#include "SandboxLayer.h"

#include <imgui.h>

namespace TerranEngine 
{
	SandboxLayer::SandboxLayer()
		: Layer("Sandbox Layer")
	{
		float positions[] =
		{
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f,
		};

		int indices[]
		{
			0, 1, 2,
			2, 3, 0
		};

		m_VertexArray.reset(new VertexArray());
		m_VertexArray->Bind();

		m_VertexBuffer.reset(new VertexBuffer(positions, sizeof(positions)));
		m_VertexBuffer->SetLayout({

			{ BufferElementType::Type::FLOAT3 }

		});

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_IndexBuffer.reset(new IndexBuffer(indices, sizeof(indices)));
		
		m_Shader.reset(new Shader("res/VertexShader.shader", "res/FragmentShader.shader"));
		m_Shader->Bind();
	}
	void SandboxLayer::OnAttach()
	{
	}

	void SandboxLayer::Update(float& time)
	{
		m_Shader->UploadFloat4("u_Color", 1.0f, 1.0f, 0.0f, 1.0f);
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

