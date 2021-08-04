#include "SandboxLayer.h"

#include <imgui.h>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <math.h>

namespace TerranEngine 
{
	SandboxLayer::SandboxLayer()
		: Layer("Sandbox Layer"), 
		m_Camera(Application::Get()->GetWindow().GetWidth() * 0.1f, Application::Get()->GetWindow().GetHeight() * 0.1f), m_Transform2(glm::vec3(0.5f, 0.0f, 0.0f)),
		m_TSheet("res/bruh2.png", 50, 50, 2)
	{
		float positions[] =
		{
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		m_TexCoords[0] = { 0.0f, 0.0f };
		m_TexCoords[1] = { 0.5f, 0.0f };
		m_TexCoords[2] = { 0.5f, 0.5f };
		m_TexCoords[3] = { 0.0f, 0.5f };

		int indices[]
		{
			0, 1, 2,
			2, 3, 0
		};	

		m_Texture = new Texture("res/bruh2.png");
		m_Texture2 = new Texture("res/test_grass.png");

		m_Font = new Font("res/OpenSans-Bold.ttf", 80);
		//TR_TRACE(m_SSheet.GetUVS(0));

		//FontUtils::LoadFont("res/OpenSans-Bold.ttf");
	}

	void SandboxLayer::OnAttach()
	{
		BatchRenderer::Init(20000);
	}

	void SandboxLayer::OnDettach()
	{
		BatchRenderer::Close();

		delete m_Texture;
		delete m_Texture2;

		delete m_Font;
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

		/*for (float x = -5.0f; x < 50.0f; x += 0.5f)
		{
			for (float y = -5.0f; y < 50.0f; y += 0.5f)
			{
				glm::vec3 pos(x * 5, y * 5, 0.0f);

				Transform transform(pos);

				BatchRenderer::AddQuad(transform.GetTransformMatrix(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0, m_Texture, m_TSheet.GetUVS(1));
			}
		}*/
		
		BatchRenderer::AddText(m_Transform1.GetTransformMatrix(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), m_Font, "e");

		//TR_TRACE(fps);


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
		m_Camera.SetViewport(Application::Get()->GetWindow().GetWidth() * 0.1f, Application::Get()->GetWindow().GetHeight() * 0.1f);

		return false;
	}

	bool SandboxLayer::OnMouseScroll(MouseScrollEvent& event)
	{
		m_ZoomLevel += event.GetYOffset() * 0.01f;
		m_Camera.SetViewport(Application::Get()->GetWindow().GetWidth() * m_ZoomLevel, Application::Get()->GetWindow().GetHeight() * m_ZoomLevel);

		return false;
	}
}
