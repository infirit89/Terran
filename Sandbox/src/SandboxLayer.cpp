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
		m_Camera(Application::Get()->GetWindow().GetWidth() * 0.1f, Application::Get()->GetWindow().GetHeight() * 0.1f), m_Transform2(glm::vec3(0.5f, 0.0f, 0.0f))
	{
		m_Renderer = CreateUnique<BatchRenderer2D>(20000);

		m_Renderer->CreateFramebuffer(1280, 790, false);

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
		
		m_Scene = CreateShared<Scene>();

		/*testEntity = m_Scene->CreateEntity();
		testEntity.AddComponent<SpriteRendererComponent>().Color = { 1.0f, 0.0f, 1.0f, 1.0f };

		Entity tEntity = m_Scene->CreateEntity();
		tEntity.GetComponent<TransformComponent>().Position = { 1.5f, 0.0f, 0.0f };
		tEntity.AddComponent<SpriteRendererComponent>().Color = { 1.0f, 1.0f, 0.0f, 1.0f };

		cameraEntity = m_Scene->CreateEntity();
		cameraEntity.AddComponent<CameraComponent>().Camera = m_Camera;*/

		m_SSerializer = SceneSerializer(m_Scene);

		Shared<Scene> newScene = CreateShared<Scene>();

		SceneSerializer serializer(newScene);
		serializer.DesirializeJson("");

		m_Scene = newScene;

		m_Font = CreateShared<Font>("res/OpenSans-Bold.ttf", 40);
	}

	void SandboxLayer::OnAttach()
	{
	}

	void SandboxLayer::OnDettach()
	{
	}

	void SandboxLayer::Update(float& time)
	{

		/* Bit fat fucking note
		* a lot here is test code
		* the framebuffer object doesn't work goddamn properly
		* 1. blending is just fucking gone
		*/ 
		
		if (m_ViewportSize.x != m_Renderer->GetFramebuffer()->Width ||
			m_ViewportSize.y != m_Renderer->GetFramebuffer()->Height) 
		{
			m_Renderer->GetFramebuffer()->Resize(m_ViewportSize.x, m_ViewportSize.y);
			m_Scene->OnResize(m_ViewportSize.x, m_ViewportSize.y);
		}
		
		if (Input::IsKeyPressed(Key::A))
			m_CameraTransform.Position.x += 10.0f * time;
		else if (Input::IsKeyPressed(Key::D))
			m_CameraTransform.Position.x -= 10.0f * time;
		if (Input::IsKeyPressed(Key::W))
			m_CameraTransform.Position.y += 10.0f * time;
		else if (Input::IsKeyPressed(Key::S))
			m_CameraTransform.Position.y -= 10.0f * time;

		if (Input::IsKeyPressed(Key::R))
			m_CameraTransform.Rotation += 10 * time;

		RenderCommand::Clear();

		RenderCommand::WireframeMode(m_Wireframe);


		m_Renderer->ResetStats();

		m_Renderer->GetFramebuffer()->Bind();
		RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		RenderCommand::Clear();
		
		//m_Scene->Update();

		m_Renderer->BeginScene(m_Camera, m_CameraTransform.GetTransformMatrix());

		m_Renderer->AddText(m_Transform1.GetTransformMatrix(), m_TextColor, m_Font, "Test test!12345345894506809@fuckmail.cok");
		m_Renderer->AddQuad(m_Transform2.GetTransformMatrix(), { 1.0f, 1.0f, 1.0f, 1.0f });


		m_Renderer->EndScene();

		m_Renderer->GetFramebuffer()->Unbind();

		stats = m_Renderer->GetStats();
		int frames = 1 / time;

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
		static bool viewPortOpen = true;

		if (ImGui::BeginMainMenuBar()) 
		{
			if (ImGui::BeginMenu("Test menu")) 
			{
				if (ImGui::MenuItem("Open Stats"))
					rendererStatsOpen = true;

				if (ImGui::MenuItem("Open viewport"))
					viewPortOpen = true;

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if (rendererStatsOpen)
		{
			ImGui::Begin("Renderer Stats", &rendererStatsOpen);

			ImGui::Text("Draw calls: %d", stats.DrawCalls);
			ImGui::Text("Total Quad count: %d", stats.GetQuadCount());
			ImGui::Text("Total Vertex count: %d", stats.VertexCount);
			ImGui::Text("Total Index count: %d", stats.IndexCount);

			ImGui::Text("Frame Time: %f ms/frame", m_Time * 1000.0f);

			ImGui::DragInt2("Renderer stress test", (int*)&m_Max, 0.1f);

			ImGui::End();
		}

		ImGui::Begin("Transform Test");

		ImGui::DragFloat3("Transform 1", (float*)&m_Transform1, 0.1f);
		ImGui::DragFloat3("Transform 2", (float*)&m_Transform2, 0.1f);

		ImGui::ColorEdit4("Color", (float*)&m_TextColor);
		
		ImGui::End();

		if (viewPortOpen) 
		{
			ImGui::Begin("Viewport", &viewPortOpen);

			ImVec2 regionAvail = ImGui::GetContentRegionAvail();

			m_ViewportSize = { regionAvail.x, regionAvail.y };

			uint32_t textureID = m_Renderer->GetFramebuffer()->GetColorAttachmentID();
			ImGui::Image((void*)textureID, regionAvail, { 0, 1 }, {1, 0});

			ImGui::End();
		}
	}

	bool SandboxLayer::KeyPressed(KeyPressedEvent& event)
	{
		if (event.GetKeyCode() == Key::R)
			TR_TRACE("pressed");

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
			case Key::I: 
			{
				if (!m_Wireframe)
					m_Wireframe = true;
				else
					m_Wireframe = false;
				break;
			}

			case Key::S:
				if(isCtrlPressed)
					m_SSerializer.SerializeJson("");
				break;
			case Key::O:
				if (isCtrlPressed) 
				{

				}
				break;
		}

		return false;
	}

	bool SandboxLayer::OnWindowResize(WindowResizeEvent& event) 
	{
		return false;
	}

	bool SandboxLayer::OnMouseScroll(MouseScrollEvent& event)
	{
		m_ZoomLevel += event.GetYOffset() * 0.005f;
		
		m_Camera.SetViewport(m_ViewportSize.x * m_ZoomLevel, m_ViewportSize.y * m_ZoomLevel);

		//auto& camComp = m_Scene->GetPrimaryCamera().GetComponent<CameraComponent>();
		//camComp.Camera.SetViewport(m_ViewportSize.x * m_ZoomLevel, m_ViewportSize.y * m_ZoomLevel);

		return false;
	}
}
