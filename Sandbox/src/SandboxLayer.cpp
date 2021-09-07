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
		BatchRenderer::ClearStats();

		BatchRenderer::Begin();

		if (Input::IsKeyPressed(Key::A)) 
			m_CameraTransform.Position.x += 10 * time;
		else if (Input::IsKeyPressed(Key::D)) 
			m_CameraTransform.Position.x -= 10 * time;
		if (Input::IsKeyPressed(Key::W))
			m_CameraTransform.Position.y += 10 * time;
		else if (Input::IsKeyPressed(Key::S))
			m_CameraTransform.Position.y -= 10 * time;

		//BatchRenderer::AddText(m_Transform1.GetTransformMatrix(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), m_Font, "FPS: " + std::to_string(frames));
		//BatchRenderer::AddText(m_Transform2.GetTransformMatrix(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), m_Font, "Bitch");
		BatchRenderer::AddQuad(m_Transform2.GetTransformMatrix(), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));


		//for (size_t y = 0; y < 200; y++)
		//{
		//	for (size_t x = 0; x < 200; x++)
		//	{
		//		Transform transform;

		//		transform.Position = { x * 10.0f, y * 10.0f, 0.0f };

		//		BatchRenderer::AddQuad(transform.GetTransformMatrix(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//	}
		//}

		BatchRenderer::EndScene(m_Camera, m_CameraTransform.GetTransformMatrix());

		stats = BatchRenderer::GetStats();
		int frames = 1 / time;
		TR_TRACE(time);

		m_Time = time;

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

		static bool rendererStatsOpen = true;

		if (ImGui::BeginMainMenuBar()) 
		{
			if (ImGui::BeginMenu("Test menu")) 
			{
				if (ImGui::MenuItem("test"))
					rendererStatsOpen = true;

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (rendererStatsOpen)
		{
			ImGui::Begin("Renderer Stats", &rendererStatsOpen);

			ImGui::Text("Draw calls: %d", stats.DrawCalls);
			ImGui::Text("Total Quad count: %d", stats.TotalQuadCount);
			ImGui::Text("Total Vertex count: %d", stats.TotalVertexCount);
			ImGui::Text("Total Index count: %d", stats.TotalIndexCount);

			ImGui::Text("Time it takes for a frame: %f", m_Time);

			ImGui::Separator();
			ImGui::Text("Batch stats:");

			for (size_t i = 0; i < stats.Batches; i++)
			{
				if (ImGui::TreeNode((void*)i, "Batch %d", i)) 
				{
					ImGui::Text("Quad count: %d", stats.BatchStats[i].GetQuadCount());
					ImGui::Text("Vertices count: %d", stats.BatchStats[i].VertexCount);
					ImGui::Text("Indices count: %d", stats.BatchStats[i].IndexCount);

					ImGui::TreePop();
				}
			}


			ImGui::End();
		}
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
