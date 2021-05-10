#include "SandboxLayer.h"

#include <imgui.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace TerranEngine 
{
	SandboxLayer::SandboxLayer()
		: Layer("Sandbox Layer"), 
		m_Camera(Application::Get()->GetWindow().GetWidth() * 0.01f, Application::Get()->GetWindow().GetHeight() * 0.01f), m_Transform2(glm::vec3(0.5f, 0.0f, 0.0f))
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
		BatchRenderer::Init(20000);
	}

	void SandboxLayer::OnDettach()
	{
		BatchRenderer::Close();
	}

	void SandboxLayer::Update(float& time)
	{

		if (Input::IsKeyPressed(Key::A))
			m_CameraTransform.Position.x += 10 * time;
		else if (Input::IsKeyPressed(Key::D))
			m_CameraTransform.Position.x -= 10 * time;
		if (Input::IsKeyPressed(Key::W))
			m_CameraTransform.Position.y -= 10 * time;
		else if (Input::IsKeyPressed(Key::S))
			m_CameraTransform.Position.y += 10 * time;

		fps = 1 / time;
		int times = 0;

		/*for (float x = -5.0f; x < 10.0f; x += 0.5f)
		{
			for (float y = -5.0f; y < 10.0f; y += 0.5f)
			{
				glm::vec3 pos(x * 3, y * 3, 0.0f);

				Transform transform(pos);

				BatchRenderer::AddQuad(transform.GetTransformMatrix(), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0, m_Texture);

				times++;
			}
		}*/

		BatchRenderer::AddQuad(m_Transform1.GetTransformMatrix(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0, m_Texture);
		TR_TRACE(times);

		TR_TRACE(fps);

		BatchRenderer::EndScene(m_Camera, m_CameraTransform.GetTransformMatrix());
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
		m_Camera.SetViewport(Application::Get()->GetWindow().GetWidth(), Application::Get()->GetWindow().GetHeight());

		return false;
	}

	bool SandboxLayer::OnMouseScroll(MouseScrollEvent& event)
	{
		m_ZoomLevel += event.GetYOffset() * 0.01f;
		m_Camera.SetViewport(Application::Get()->GetWindow().GetWidth() * m_ZoomLevel, Application::Get()->GetWindow().GetHeight() * m_ZoomLevel);
		return false;
	}
}
