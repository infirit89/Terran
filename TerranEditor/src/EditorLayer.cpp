#include "EditorLayer.h"

#include "UI/UI.h"
#include "EditorConsoleSink.h"

#include "EditorPanels/SceneHierarchyPanel.h"
#include "EditorPanels/PropertiesPanel.h"
#include "EditorPanels/SceneViewPanel.h"
#include "EditorPanels/GameViewPanel.h"
#include "EditorPanels/ContentPanel.h"
#include "EditorPanels/ECSPanel.h"
#include "EditorPanels/LogPanel.h"
#include "EditorPanels/SettingsPanel.h"
#include "SelectionManager.h"

#include "EditorResources.h"

#include "Terran.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>

#include <filesystem>

#include <IconFontAwesome6.h>

#pragma warning(push, 0)
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

namespace TerranEditor
{
#define LOG_PANEL_NAME "LogPanel"
#define SCENE_HIERARCHY_PANEL_NAME "SceneHierarchyPanel"
#define PROPERTIES_PANEL_NAME "PropertiesPanel"
#define ECS_PANEL_NAME "ECSPanel"
#define CONTENT_PANEL_NAME "ContentPanel"
#define SCENE_VIEW_PANEL_NAME "SceneViewPanel"
#define SETTINGS_PANEL_NAME "SettingPanel"

	EditorLayer* EditorLayer::s_Instance;

	EditorLayer::EditorLayer(const std::string& projectPath)
		: Layer("Editor"), m_EditorCamera(), m_ProjectPath(projectPath) 
	{
		s_Instance = this;

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
	}

	void EditorLayer::OnAttach()
	{
		EditorResources::Init();
		UI::SetupImGuiStyle();

		FontAtlas fontAtlas;

		ImGuiIO& io = ImGui::GetIO();

		ImFontConfig config;
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto/Roboto-Regular.ttf", 15.0f, &config);
		config.MergeMode = true;
		io.Fonts->Build();
		io.IniFilename = "Resources/TerranEditorSettings.ini";

		m_EditorScene = SceneManager::CreateEmpyScene();
		Entity cameraEntity = m_EditorScene->CreateEntity("Camera");
		CameraComponent& cameraComponent = cameraEntity.AddComponent<CameraComponent>();

		cameraComponent.Camera.SetViewport(m_ViewportSize.x, m_ViewportSize.y);
		
		SceneManager::SetCurrentScene(m_EditorScene);

		// ***** Panel Setup *****
		m_PanelManager = CreateShared<PanelManager>();
		m_PanelManager->AddPanel<LogPanel>(LOG_PANEL_NAME);
		m_PanelManager->AddPanel<ContentPanel>(CONTENT_PANEL_NAME);
		Shared<SceneViewPanel> sceneViewPanel = m_PanelManager->AddPanel<SceneViewPanel>(SCENE_VIEW_PANEL_NAME);
		sceneViewPanel->SetOpenSceneCallback([this](const std::filesystem::path& sceneName, glm::vec2 sceneViewport) { OpenScene(sceneName, sceneViewport); });
		sceneViewPanel->SetViewportSizeChangedCallback([this](glm::vec2 viewportSize) {  OnViewportSizeChanged(viewportSize); });
		m_PanelManager->AddPanel<SceneHierarchyPanel>(SCENE_HIERARCHY_PANEL_NAME);
		m_PanelManager->AddPanel<ECSPanel>(ECS_PANEL_NAME);
		m_PanelManager->AddPanel<PropertiesPanel>(PROPERTIES_PANEL_NAME);
		m_PanelManager->SetScene(SceneManager::GetCurrentScene());
        Shared<SettingsPanel> settingsPanel = m_PanelManager->AddPanel<SettingsPanel>(SETTINGS_PANEL_NAME);
        settingsPanel->SetOpen(false);
		// ***********************

		if (m_ProjectPath.empty())
			OpenProject("SandboxProject/");
		else
			OpenProject(m_ProjectPath);

		FramebufferParameters editorFramebufferParams;
		editorFramebufferParams.ColorAttachemnts = { FramebufferColorAttachmentType::RGBA, FramebufferColorAttachmentType::Red32Integer };
		editorFramebufferParams.DepthAttachment = { FramebufferDepthAttachmentType::Depth24Stencil8 };

		m_EditorSceneRenderer = CreateShared<SceneRenderer>(editorFramebufferParams);
		
		ScriptEngine::SetLogCallback([this](const std::string& message, spdlog::level::level_enum level) { OnScriptEngineLog(message, level); });

		sceneViewPanel->SetSceneRenderer(m_EditorSceneRenderer);
        ScriptEngine::LoadAppAssembly();

		FileSystem::SetDirectoryToWatch(Project::GetAssetPath());
		FileSystem::StartWatch();
	}

	void EditorLayer::OnDettach()
	{
		FileSystem::StopWatch();
		EditorResources::Shutdown();
	}

	void EditorLayer::Update(Time& time)
	{		
		TR_PROFILE_FUNCN("EditorLayer::Update");

		BatchRenderer2D::ResetStats();
		m_EditorCamera.Update(time);

		if (/*m_SceneViewPanel.IsVisible() &&*/ SceneManager::GetCurrentScene())
		{
			switch (m_SceneState) 
			{
			case SceneState::Edit: 
			{
				SceneManager::GetCurrentScene()->UpdateEditor();
				SceneManager::GetCurrentScene()->OnRenderEditor(m_EditorSceneRenderer, m_EditorCamera, m_EditorCamera.GetView());
				
				//m_SceneViewPanel.SetRenderTextureID(m_EditorSceneRenderer->GetFramebuffer()->GetColorAttachmentID());
				//m_SceneViewPanel.SetFramebuffer(m_EditorSceneRenderer->GetFramebuffer());

				break;
			}
			case SceneState::Play: 
			{
				SceneManager::GetCurrentScene()->Update(time);
				SceneManager::GetCurrentScene()->OnRender(m_EditorSceneRenderer);

				//m_SceneViewPanel.SetRenderTextureID(m_RuntimeSceneRenderer->GetFramebuffer()->GetColorAttachmentID());
				//m_SceneViewPanel.SetFramebuffer(m_RuntimeSceneRenderer->GetFramebuffer());

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
		m_PanelManager->OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(TR_EVENT_BIND_FN(EditorLayer::OnKeyPressedEvent));
		dispatcher.Dispatch<WindowCloseEvent>(TR_EVENT_BIND_FN(EditorLayer::OnWindowCloseEvent));
		dispatcher.Dispatch<GamepadConnectedEvent>(TR_EVENT_BIND_FN(EditorLayer::OnGamepadConnectedEvent));
		dispatcher.Dispatch<GamepadDisconnectedEvent>(TR_EVENT_BIND_FN(EditorLayer::OnGamepadDisconnectedEvent));
	}
	
	bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& kEvent)
	{
		bool shiftPressed = Input::IsKeyDown(Key::LeftShift) || Input::IsKeyDown(Key::RightShift);
		bool ctrlPressed = Input::IsKeyDown(Key::LeftControl) || Input::IsKeyDown(Key::RightControl);

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

	bool EditorLayer::OnWindowCloseEvent(WindowCloseEvent& wEvent)
	{
		if(m_SceneState == SceneState::Play)
			OnSceneStop();

		AssetManager::WriteAssetInfosToFile();

		return false;
	}

	bool EditorLayer::OnGamepadConnectedEvent(GamepadConnectedEvent& gEvent)
	{
		TR_CLIENT_INFO("Connected controller: {0}", Input::GetControllerName(gEvent.GetGamepadID()));
		return false;
	}

	bool EditorLayer::OnGamepadDisconnectedEvent(GamepadDisconnectedEvent& gEvent)
	{
		TR_CLIENT_INFO("Disconnected controller: {0}", Input::GetControllerName(gEvent.GetGamepadID()));
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
                    m_PanelManager->SetPanelOpen(PROPERTIES_PANEL_NAME, true);

                if (ImGui::MenuItem("Scene Hierarchy"))
                    m_PanelManager->SetPanelOpen(SCENE_HIERARCHY_PANEL_NAME, true);

                if (ImGui::MenuItem("Scene View"))
                    m_PanelManager->SetPanelOpen(SCENE_VIEW_PANEL_NAME, true);

                if (ImGui::MenuItem("Performance"))
                    m_PerformancePanelOpen = true;

                if (ImGui::MenuItem("Content Browser"))
                    m_PanelManager->SetPanelOpen(CONTENT_PANEL_NAME, true);

                if (ImGui::MenuItem("Renderer Stats"))
                    m_RendererStatsPanelOpen = true;

                if (ImGui::MenuItem("ECS Panel"))
                    m_PanelManager->SetPanelOpen(ECS_PANEL_NAME, true);

                if (ImGui::MenuItem("Logger"))
                    m_PanelManager->SetPanelOpen(LOG_PANEL_NAME, true);

                if(ImGui::MenuItem("Settings"))
                    m_PanelManager->SetPanelOpen(SETTINGS_PANEL_NAME, true);

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
					//m_RuntimeSceneRenderer->SetShowColliders(m_ShowColliders);
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();
	}

	void EditorLayer::OnScenePlay()
	{
		Shared<LogPanel> logPanel = m_PanelManager->GetPanel<LogPanel>("LogPanel");
		if (logPanel->IsClearOnPlay())
			logPanel->ClearMessageBuffer();

		m_SceneState = SceneState::Play;

		UUID tempSelected = SelectionManager::GetSelectedID();
		SelectionManager::Deselect();
		SceneManager::SetCurrentScene(Scene::CopyScene(m_EditorScene));
		SceneManager::GetCurrentScene()->OnResize(m_ViewportSize.x, m_ViewportSize.y);

		//m_SceneHierarchyPanel.SetScene(m_ActiveScene);
		m_PanelManager->SetScene(SceneManager::GetCurrentScene());
		SceneManager::GetCurrentScene()->StartRuntime();

		//m_SceneHierarchyPanel.SetSelected(m_Selected);

		if(tempSelected)
			SelectionManager::Select(tempSelected);
	}

	void EditorLayer::OnSceneStop()
	{
		UUID tempSelected = SelectionManager::GetSelectedID();
		SelectionManager::Deselect();
		m_SceneState = SceneState::Edit;
		SceneManager::GetCurrentScene()->StopRuntime();
		//SceneManager::RemoveScene(SceneManager::CurrentScene->GetID());
		//m_SceneHierarchyPanel.SetScene(m_ActiveScene);
		m_PanelManager->SetScene(m_EditorScene);
		//m_SceneHierarchyPanel.SetSelected(m_EditModeSelected);
		SceneManager::SetCurrentScene(m_EditorScene);

		if(tempSelected)
			SelectionManager::Select(tempSelected);
	}

	void EditorLayer::OnScriptEngineLog(const std::string& message, spdlog::level::level_enum level)
	{
		switch (level)
		{
		case spdlog::level::trace:
		case spdlog::level::debug:
		case spdlog::level::info:
			TR_CLIENT_INFO(message);
			break;
		case spdlog::level::warn:
			TR_CLIENT_WARN(message);
			break;
		case spdlog::level::err:
		case spdlog::level::critical:
			TR_CLIENT_ERROR(message);
			break;
		}
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
			//m_RuntimeSceneRenderer->OnResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			SceneManager::GetCurrentScene()->OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorSceneRenderer->OnResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

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

		m_PanelManager->ImGuiRender();

		// Renderer stats
		{
			if (m_RendererStatsPanelOpen) 
			{
				ImGui::Begin("Renderer Stats", &m_RendererStatsPanelOpen);
			
				BatchRendererStats stats = BatchRenderer2D::GetStats();
				ImGui::Indent(4.0f);
				ImGui::Text("Draw calls: %d", stats.DrawCalls);
				ImGui::Text("Total Vertex count: %d", stats.VertexCount);
				ImGui::Text("Total Index count: %d", stats.IndexCount);
				ImGui::Unindent(4.0f);

				ImGui::End();
			}
		}

		// Performance
		{
			if (m_PerformancePanelOpen) 
			{
				ImGui::Begin("Performance", &m_PerformancePanelOpen);

				ImGui::Text("Frame Time: %.01f ms/frame", m_Frametime);
			
				for (auto result : Profiler::Get().GetResults())
					ImGui::Text("%s: %f ms", result.Name.c_str(), result.ElapsedTime);

				ImGui::End();
			}
		}

		{
			// NOTE: temporary toolbar
			ImGuiWindowFlags toolbarFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
											ImGuiWindowFlags_NoTitleBar;
            
			ImGui::Begin("##toolbar", nullptr, toolbarFlags);

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
		std::filesystem::path scenePath = FileSystem::SaveFile("Terran Scene\0*.terran\0");
		if (!scenePath.empty())
		{
			m_CurrentScenePath = scenePath;
			SceneSerializer sSerializer(SceneManager::GetCurrentScene());
			sSerializer.SerializeJson(scenePath);

			m_CurrentScenePath = scenePath;
		}
	}

	void EditorLayer::NewScene()
	{
		m_EditorScene = SceneManager::CreateEmpyScene();
		CameraComponent& cameraComponent = m_EditorScene->CreateEntity("Camera").AddComponent<CameraComponent>();
		cameraComponent.Primary = true;
		m_EditorScene->OnResize(m_ViewportSize.x, m_ViewportSize.y);

		SceneManager::SetCurrentScene(m_EditorScene);
		//m_SceneHierarchyPanel.SetScene(m_ActiveScene);
		/*m_ECSPanel.SetContext(m_ActiveScene);
		m_SceneViewPanel.SetContext(m_ActiveScene);*/
		m_PanelManager->SetScene(SceneManager::GetCurrentScene());
	}

	void EditorLayer::OpenScene()
	{
		std::filesystem::path scenePath = FileSystem::OpenFile("Terran Scene\0*.terran\0");		
		OpenScene(scenePath, m_ViewportSize);
	}

    static bool IsValidAssetPath(const std::filesystem::path& path)
    {
        std::filesystem::path tempPath = path;
        while(tempPath.has_parent_path())
        {
            if(tempPath.stem() == "Assets")
                return true;

            tempPath = tempPath.parent_path();
        }

        return false;    
    }

	void EditorLayer::OpenScene(const std::filesystem::path& scenePath, const glm::vec2& viewportSize)
	{
		std::filesystem::path path = scenePath.empty() ? m_CurrentScenePath : scenePath;

        //if(!IsValidAssetPath(path))
            //return;

		if (!path.empty()) 
		{
			if (scenePath.extension() != ".terran")
			{
				TR_ERROR("Couldn't load the file");
				return;
			}

			std::string jsonData = SceneSerializer::ReadJson(AssetManager::GetFileSystemPath(scenePath).string());
			if (jsonData != "")
			{
				Shared<Scene> newScene = SceneManager::CreateEmpyScene();
				SceneSerializer sSerializer(newScene);
				if (sSerializer.DesirializeJson(jsonData))
				{
					if (SceneManager::GetCurrentScene()->IsPlaying())
						OnSceneStop();

					SelectionManager::Deselect();
					m_EditorScene = newScene;
					m_EditorScene->OnResize(viewportSize.x, viewportSize.y);
					
					SceneManager::SetCurrentScene(newScene);

					m_PanelManager->SetScene(SceneManager::GetCurrentScene());

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
			SceneSerializer sSerializer(SceneManager::GetCurrentScene());
			sSerializer.SerializeJson(m_CurrentScenePath);
		}
	}

    void EditorLayer::OpenProject(const std::filesystem::path& projectPath)
    {
        // TODO: close project if theres an already active one
        Shared<Project> project = CreateShared<Project>();
        Project::SetActive(project);
        project->ProjectPath = projectPath;

		ProjectSerializer projectSerializer(project);
		projectSerializer.DeserializePhysicsSettings();

		AssetManager::LoadAssetInfos();

		m_PanelManager->OnProjectChanged(projectPath);
    }
}
