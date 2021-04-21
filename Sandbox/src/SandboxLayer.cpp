#include "SandboxLayer.h"

#include <imgui.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace TerranEngine 
{
	SandboxLayer::SandboxLayer()
		: Layer("Sandbox Layer"), 
		m_Camera(Application::Get()->GetWindow().GetWidth() * 0.01f, Application::Get()->GetWindow().GetHeight() * 0.01f)
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
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	
		m_Texture = new Texture("res/test_grass.png");
	}

	void SandboxLayer::OnAttach()
	{
		BatchRenderer::Init(2000);
	}

	void SandboxLayer::OnDettach()
	{
		BatchRenderer::Close();
	}

	void SandboxLayer::Update(float& time)
	{
		BatchRenderer::BeginScene(m_Camera, m_ViewMatrix);
		BatchRenderer::Draw(m_Transform.GetTransformMatrix(), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		BatchRenderer::EndScene();
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
