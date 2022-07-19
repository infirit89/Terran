#include "EditorLayer.h"

#include "UI/UI.h"
#include "EditorConsoleSink.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>

#include <filesystem>

#pragma warning(push, 0)
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

namespace TerranEditor
{
	EditorLayer* EditorLayer::s_Instance;

	EditorLayer::EditorLayer()
		: Layer("Editor"), m_EditorCamera() 
	{
		s_Instance = this;
	}

	void EditorLayer::OnAttach()
	{
		Project::Init("", "SandboxProject");
		FontAtlas fontAtlas;

		ImGuiIO& io = ImGui::GetIO();

		ImFontConfig config;
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto/Roboto-Regular.ttf", 15.0f, &config);
		config.MergeMode = true;
		io.Fonts->Build();
		io.IniFilename = "Resources/TerranEditorSettings.ini";

		std::vector<spdlog::sink_ptr> clientSinks
		{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Terran_Editor.log", true),
			std::make_shared<EditorConsoleSink>()
		};

		clientSinks[0]->set_pattern("%^[%T] %v%$");
		clientSinks[1]->set_pattern("%^[%T] %v%$");

		Shared<spdlog::logger> clientLogger = CreateShared<spdlog::logger>("Console", clientSinks.begin(), clientSinks.end());
		clientLogger->set_level(spdlog::level::trace);

		Log::SetClientLogger(clientLogger);

		m_EditorScene = Scene::CreateEmpty();
		Entity cameraEntity = m_EditorScene->CreateEntity("Camera");
		TR_TRACE((uint32_t)cameraEntity);
		CameraComponent& cameraComponent = cameraEntity.AddComponent<CameraComponent>();

		// TODO: should add an on component added function
		cameraComponent.Camera.SetViewport(m_ViewportSize.x, m_ViewportSize.y);
		
		m_ActiveScene = m_EditorScene;

		// ***** Panel Setup *****
		m_ContentPanel = ContentPanel(m_ResPath);
		
		m_SceneViewPanel.SetOpenSceneCallback([this](const char* sceneName, glm::vec2 sceneViewport) { OpenScene(sceneName, sceneViewport); });
		m_SceneViewPanel.SetViewportSizeChangedCallback([this](glm::vec2 viewportSize) {  OnViewportSizeChanged(viewportSize); });
		m_SceneViewPanel.SetSelectedChangedCallback([this](Entity entity) { m_SceneHierarchyPanel.SetSelected(entity); });
		m_SceneViewPanel.SetContext(m_ActiveScene);

		m_SceneHierarchyPanel.SetOnSelectedChangedCallback([this](Entity entity) { OnSelectedChanged(entity); });
		m_SceneHierarchyPanel.SetScene(m_ActiveScene);
		
		m_ECSPanel.SetContext(m_ActiveScene);
		// ***********************

		FramebufferParameters editorFramebufferParams;
		editorFramebufferParams.ColorAttachemnts = { FramebufferColorAttachmentType::RGBA, FramebufferColorAttachmentType::Red32Integer };
		editorFramebufferParams.DepthAttachment = { FramebufferDepthAttachmentType::Depth24Stencil8 };

		m_EditorSceneRenderer = CreateShared<SceneRenderer>(editorFramebufferParams);
		
		FramebufferParameters runtimeFramebufferParams;
		runtimeFramebufferParams.ColorAttachemnts = { FramebufferColorAttachmentType::RGBA };
		runtimeFramebufferParams.DepthAttachment = { FramebufferDepthAttachmentType::Depth24Stencil8 };

		m_RuntimeSceneRenderer = CreateShared<SceneRenderer>(runtimeFramebufferParams);

		ScriptEngine::Initialize();
		Physics2D::Initialize();
	}

	void EditorLayer::OnDettach()
	{
		Project::Uninitialize();
		ScriptEngine::Shutdown();
		Physics2D::Shutdown();
	}

	void EditorLayer::Update(Time& time)
	{
		TR_PROFILE_FUNCN("EditorLayer::Update");

		BatchRenderer2D::Get()->ResetStats();
		m_EditorCamera.Update(time);

		if (m_SceneViewPanel.IsVisible() && m_ActiveScene) 
		{
			switch (m_SceneState) 
			{
			case SceneState::Edit: 
			{
				m_ActiveScene->UpdateEditor();
				m_ActiveScene->OnRenderEditor(m_EditorSceneRenderer, m_EditorCamera, m_EditorCamera.GetView());
				
				//m_SceneViewPanel.SetRenderTextureID(m_EditorSceneRenderer->GetFramebuffer()->GetColorAttachmentID());
				m_SceneViewPanel.SetFramebuffer(m_EditorSceneRenderer->GetFramebuffer());

				break;
			}
			case SceneState::Play: 
			{
				auto primaryCamera = m_ActiveScene->GetPrimaryCamera();

				glm::vec4 backgroundColor = glm::vec4(0.0f);

				if (primaryCamera)
					backgroundColor = primaryCamera.GetComponent<CameraComponent>().BackgroundColor;

				m_RuntimeSceneRenderer->SetClearColor(backgroundColor);

				m_ActiveScene->Update(time);
				m_ActiveScene->OnRender(m_RuntimeSceneRenderer);

				//m_SceneViewPanel.SetRenderTextureID(m_RuntimeSceneRenderer->GetFramebuffer()->GetColorAttachmentID());
				m_SceneViewPanel.SetFramebuffer(m_RuntimeSceneRenderer->GetFramebuffer());

				break;
			}
			}
		}

#if 0
		if (m_GameView.IsVisible() && SceneManager::GetCurrentScene()) 
		{
			if ((m_GameView.GetViewportSize().x != m_GameSceneRenderer->GetViewportWidth() ||
				m_GameView.GetViewportSize().y != m_GameSceneRenderer->GetViewportHeight()) &&
				m_GameView.GetViewportSize().x > 0 && m_GameView.GetViewportSize().y > 0)
			{
				m_GameSceneRenderer->OnResize(m_GameView.GetViewportSize().x, m_GameView.GetViewportSize().y);
				SceneManager::GetCurrentScene()->OnResize(m_GameView.GetViewportSize().x, m_GameView.GetViewportSize().y);
			}

			auto primaryCamera = SceneManager::GetCurrentScene()->GetPrimaryCamera();

			glm::vec4 backgroundColor = glm::vec4(0.0f);

			if (primaryCamera)
				backgroundColor = primaryCamera.GetComponent<CameraComponent>().BackgroundColor;

			m_GameSceneRenderer->SetClearColor(backgroundColor);

			SceneManager::GetCurrentScene()->Update();
			SceneManager::GetCurrentScene()->OnRender(m_GameSceneRenderer);

			m_GameView.SetRenderTextureID(m_GameSceneRenderer->GetFramebuffer()->GetColorAttachmentID());
		}
#endif

		m_Frametime = time.GetDeltaTimeMS();
	}


	void EditorLayer::OnEvent(Event& event)
	{
		m_EditorCamera.OnEvent(event);
		m_SceneViewPanel.OnEvent(event);
		m_SceneHierarchyPanel.OnEvent(event);

		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<KeyPressedEvent>(TR_EVENT_BIND_FN(EditorLayer::OnKeyPressedEvent));
	}
	
	bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& kEvent)
	{
		bool shiftPressed = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		bool ctrlPressed = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);

		if (kEvent.GetRepeatCount() > 0)
			return false;

		switch (kEvent.GetKeyCode())
		{
		case Key::S:
		{
			if (ctrlPressed) 
			{
				if (shiftPressed)
					SaveSceneAs();
				else
					SaveScene();
			}
			break;
		}
		case Key::O:
		{
			if (ctrlPressed)
				OpenScene();
			break;
		}
		case Key::N:
		{
			if (ctrlPressed)
				NewScene();
			break;
		}
		}

		return false;
	}
	
	void EditorLayer::ShowDockspace() 
	{
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoWindowMenuButton;
			
		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();

		// set the dock space's position to that of the main viewport
		ImGui::SetNextWindowPos(viewport->WorkPos);

		// set the dock space's size to that of the main viewport 
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Terran Editor", (bool*)true, window_flags);
		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("TerranEditorDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", "Ctrl+O"))
					OpenScene();
				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveSceneAs();
				if (ImGui::MenuItem("Save", "Ctrl+S"))
					SaveScene();
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View")) 
			{
				if (ImGui::MenuItem("Properties"))
					m_PropertiesPanel.SetOpen(true);

				if (ImGui::MenuItem("Scene Hierarchy"))
					m_SceneHierarchyPanel.SetOpen(true);

				if (ImGui::MenuItem("Editor View"))
					m_SceneViewPanel.SetOpen(true);

				if (ImGui::MenuItem("Performance"))
					m_PerformanceOpen = true;

				if (ImGui::MenuItem("Content Browser"))
					m_ContentPanel.SetOpen(true);

				if (ImGui::MenuItem("Renderer Stats"))
					m_RendererStatsOpen = true;

				if (ImGui::MenuItem("ECS Panel"))
					m_ECSPanel.SetOpen(true);

				if (ImGui::MenuItem("Logger"))
					m_LogPanel.SetOpen(true);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools")) 
			{
				if (ImGui::MenuItem("Reload C# Assembly"))
					ScriptEngine::ReloadAppAssembly();
				
				if (ImGui::MenuItem("Show colliders", NULL, m_ShowColliders)) 
				{
					m_ShowColliders = !m_ShowColliders;
					m_EditorSceneRenderer->SetShowColliders(m_ShowColliders);
					m_RuntimeSceneRenderer->SetShowColliders(m_ShowColliders);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_LogPanel.IsClearOnPlay())
			m_LogPanel.ClearMessageBuffer();

		m_SceneState = SceneState::Play;
		m_ActiveScene = Scene::CopyScene(m_EditorScene);
		m_ActiveScene->OnResize(m_ViewportSize.x, m_ViewportSize.y);

		m_SceneHierarchyPanel.SetScene(m_ActiveScene);
		m_ECSPanel.SetContext(m_ActiveScene);
		m_SceneViewPanel.SetContext(m_ActiveScene);
		m_ActiveScene->StartRuntime();

		m_SceneHierarchyPanel.SetSelected(m_Selected);
		m_EditModeSelected = m_Selected;
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;
		m_ActiveScene->StopRuntime();
		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetScene(m_ActiveScene);
		m_ECSPanel.SetContext(m_ActiveScene);
		m_SceneViewPanel.SetContext(m_ActiveScene);

		m_SceneHierarchyPanel.SetSelected(m_EditModeSelected);
	}

	void EditorLayer::OnSelectedChanged(Entity newSelected)
	{
		m_Selected = newSelected;
	}

	void EditorLayer::OnViewportSizeChanged(glm::vec2 newViewportSize)
	{
		m_ViewportSize = newViewportSize;

		switch (m_SceneState)
		{
		case TerranEditor::SceneState::Edit: 
		{
			m_EditorSceneRenderer->OnResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
			break;
		}
		case TerranEditor::SceneState::Play:
		{
			m_RuntimeSceneRenderer->OnResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_ActiveScene->OnResize(m_ViewportSize.x, m_ViewportSize.y);

			break;
		}
		}
	}

	void EditorLayer::ImGuiRender()
	{
		ImGuizmo::SetOrthographic(true);
		ImGuizmo::BeginFrame();

		ShowDockspace();

		ImGui::ShowDemoWindow();

		m_SceneHierarchyPanel.ImGuiRender();
		m_SceneViewPanel.ImGuiRender(m_Selected, m_EditorCamera);

		m_PropertiesPanel.ImGuiRender(m_Selected);
		m_ECSPanel.ImGuiRender();
		m_LogPanel.ImGuiRender();
		m_ContentPanel.ImGuiRender();

		// Renderer stats
		{
			if (m_RendererStatsOpen) 
			{
				ImGui::Begin("Renderer Stats", &m_RendererStatsOpen);
			
				BatchRendererStats stats = BatchRenderer2D::Get()->GetStats();
				ImGui::Indent(4.0f);
				ImGui::Text("Draw calls: %d", stats.DrawCalls);
				ImGui::Text("Total Quad count: %d", stats.GetQuadCount());
				ImGui::Text("Total Vertex count: %d", stats.VertexCount);
				ImGui::Text("Total Index count: %d", stats.IndexCount);
				ImGui::Unindent(4.0f);

				ImGui::End();
			}
		}

		// Performance
		{
			if (m_PerformanceOpen) 
			{
				ImGui::Begin("Performance", &m_PerformanceOpen);

				ImGui::Text("Frame Time: %.01f ms/frame", m_Frametime);
			
				for (auto result : Profiler::Get().GetResults())
					ImGui::Text("%s: %f ms", result.Name.c_str(), result.ElapsedTime);

				ImGui::End();
			}
		}

		{
			// NOTE: temporary toolbar
			ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

			ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));

			if (ImGui::Button(m_SceneState == SceneState::Edit ? "Play" : "Stop"))
			{
				if (m_SceneState == SceneState::Edit)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}

			ImGui::End();
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::filesystem::path scenePath = FileUtils::SaveFile("Terran Scene\0*.terran\0");
		if (!scenePath.empty())
		{
			m_CurrentScenePath = scenePath;
			SceneSerializer sSerializer(m_ActiveScene);
			sSerializer.SerializeJson(scenePath);

			m_CurrentScenePath = scenePath;
		}
	}

	void EditorLayer::NewScene()
	{
		m_EditorScene = Scene::CreateEmpty();
		CameraComponent& cameraComponent = m_EditorScene->CreateEntity("Camera").AddComponent<CameraComponent>();
		cameraComponent.Primary = true;
		m_EditorScene->OnResize(m_ViewportSize.x, m_ViewportSize.y);

		m_ActiveScene = m_EditorScene;
		m_SceneHierarchyPanel.SetScene(m_ActiveScene);
		m_ECSPanel.SetContext(m_ActiveScene);
		m_SceneViewPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::filesystem::path scenePath = FileUtils::OpenFile("Terran Scene\0*.terran\0");		
		OpenScene(scenePath, m_ViewportSize);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& scenePath, const glm::vec2& viewportSize)
	{
		std::filesystem::path path = scenePath.empty() ? m_CurrentScenePath : scenePath;

		if (!path.empty()) 
		{
			if (scenePath.extension() != ".terran")
			{
				TR_ERROR("Couldn't load the file");
				return;
			}

			std::string& jsonData = SceneSerializer::ReadJson(scenePath.string());
			if (jsonData != "")
			{
				Shared<Scene> newScene = Scene::CreateEmpty();
				SceneSerializer sSerializer(newScene);
				if (sSerializer.DesirializeJson(jsonData))
				{
					m_EditorScene = newScene;
					m_EditorScene->OnResize(viewportSize.x, viewportSize.y);
					
					m_ActiveScene = newScene;
					m_SceneHierarchyPanel.SetScene(m_ActiveScene);
					m_ECSPanel.SetContext(m_ActiveScene);
					m_SceneViewPanel.SetContext(m_ActiveScene);

					m_Selected = {};

					m_CurrentScenePath = scenePath;
				}
			}
		}
	}

	void EditorLayer::SaveScene()
	{
		if (m_CurrentScenePath.empty())
			SaveSceneAs();
		else 
		{
			SceneSerializer sSerializer(m_ActiveScene);
			sSerializer.SerializeJson(m_CurrentScenePath);
		}
	}
}
