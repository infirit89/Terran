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
		BatchRenderer::BeginScene(m_Camera, m_CameraTransform.GetTransformMatrix());

		if (Input::IsKeyPressed(Key::A))
			m_CameraTransform.Position.x -= 1;
		else if (Input::IsKeyPressed(Key::D))
			m_CameraTransform.Position.x += 1;
		if (Input::IsKeyPressed(Key::W))
			m_CameraTransform.Position.y += 1;
		else if (Input::IsKeyPressed(Key::S))
			m_CameraTransform.Position.y -= 1;



		for (size_t i = 0; i < 10; i++)
		{
			glm::vec3 pos(i * 1.1f, 0.0f, 0.0f);
			
			Transform transform(pos);

			BatchRenderer::Draw(transform.GetTransformMatrix(), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

		}

		BatchRenderer::EndScene();
	}

	void SandboxLayer::OnEvent(Event& event)
	{
		EventDispatcher dispather(event);
		dispather.Dispatch<KeyPressedEvent>(TR_EVENT_BIND_FN(SandboxLayer::KeyPressed));
		dispather.Dispatch<WindowResizeEvent>(TR_EVENT_BIND_FN(SandboxLayer::OnWindowResize));
		dispather.Dispatch<MouseScrollEvent>(TR_EVENT_BIND_FN(SandboxLayer::OnMouseScroll));
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

	bool SandboxLayer::OnWindowResize(WindowResizeEvent& event) 
	{
		m_Camera.SetViewport(Application::Get()->GetWindow().GetWidth() * m_ZoomLevel, Application::Get()->GetWindow().GetHeight() * m_ZoomLevel);

		return false;
	}

	bool SandboxLayer::OnMouseScroll(MouseScrollEvent& event)
	{
		m_ZoomLevel += event.GetYOffset() * 0.001f;
		m_Camera.SetViewport(Application::Get()->GetWindow().GetWidth() * m_ZoomLevel, Application::Get()->GetWindow().GetHeight() * m_ZoomLevel);
		return false;
	}
}
