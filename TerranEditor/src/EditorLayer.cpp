#include "EditorLayer.h"

#include "UI/UI.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>

#include <filesystem>

namespace TerranEditor
{
    static void CopyAssembly(const std::filesystem::path& source, const std::filesystem::path& destination);

	EditorLayer::EditorLayer()
		: Layer("Editor"), m_EditorCamera()
	{
        m_ContentPanel = ContentPanel(m_ResPath);

		m_EditorSceneRenderer = CreateShared<SceneRenderer>();
		m_GameSceneRenderer = CreateShared<SceneRenderer>();

        m_EditorScene = CreateShared<Scene>();
        Entity cameraEntity = m_EditorScene->CreateEntity("Camera");
        TR_TRACE((uint32_t)cameraEntity);
        CameraComponent& cameraComponent = cameraEntity.AddComponent<CameraComponent>();

        // TODO: should add an on component added function
        cameraComponent.Camera.SetViewport(m_SceneView.GetViewportSize().x, m_SceneView.GetViewportSize().y);
        SceneManager::SetCurrentScene(m_EditorScene);

        m_SHierarchy.SetScene(SceneManager::GetCurrentScene());
        m_ECSPanel.SetContext(SceneManager::GetCurrentScene());

        m_ScriptAssemblyPath = "Resources/Scripts/";

        CopyAssembly((m_ScriptAssemblyPath / "Temp/TerranScriptCore.dll"), m_ScriptAssemblyPath);

        ScriptEngine::Init("Resources/Scripts/TerranScriptCore.dll");

        ScriptBindings::Bind();
	}

	void EditorLayer::OnAttach()
	{
        ImGuiIO& io = ImGui::GetIO();

        ImFontConfig config;

        io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/Roboto/Roboto-Regular.ttf", 15.0f, &config);

        config.MergeMode = true;

        io.Fonts->Build();

        io.IniFilename = "Resources/TerranEditorSettings.ini";
	}

	void EditorLayer::OnDettach()
	{
        ScriptEngine::CleanUp();
	}

	void EditorLayer::Update(Time& time)
	{
        TR_PROFILE_FUNCN("EditorLayer::Update");

        BatchRenderer2D::Get()->ResetStats();
        m_EditorCamera.Update(time);

        if (m_SceneView.IsVisible() && SceneManager::GetCurrentScene()) 
        {
            switch (m_SceneState) 
            {
            case SceneState::Edit: 
            {
                if ((m_SceneView.GetViewportSize().x != m_EditorSceneRenderer->GetViewportWidth() ||
                    m_SceneView.GetViewportSize().y != m_EditorSceneRenderer->GetViewportHeight()) &&
                    m_SceneView.GetViewportSize().x > 0 && m_SceneView.GetViewportSize().y > 0)
                {
                    m_EditorSceneRenderer->OnResize(m_SceneView.GetViewportSize().x, m_SceneView.GetViewportSize().y);
                    m_EditorCamera.OnViewportResize(m_SceneView.GetViewportSize().x, m_SceneView.GetViewportSize().y);
                }

                SceneManager::GetCurrentScene()->UpdateEditor();
                SceneManager::GetCurrentScene()->OnRenderEditor(m_EditorSceneRenderer, m_EditorCamera, m_EditorCamera.GetView());

                m_SceneView.SetRenderTextureID(m_EditorSceneRenderer->GetFramebuffer()->GetColorAttachmentID());

                break;
            }
            case SceneState::Play: 
            {
                if ((m_SceneView.GetViewportSize().x != m_GameSceneRenderer->GetViewportWidth() ||
                    m_SceneView.GetViewportSize().y != m_GameSceneRenderer->GetViewportHeight()) &&
                    m_SceneView.GetViewportSize().x > 0 && m_SceneView.GetViewportSize().y > 0)
                {
                    m_GameSceneRenderer->OnResize(m_SceneView.GetViewportSize().x, m_SceneView.GetViewportSize().y);
                    SceneManager::GetCurrentScene()->OnResize(m_SceneView.GetViewportSize().x, m_SceneView.GetViewportSize().y);
                }

                auto primaryCamera = SceneManager::GetCurrentScene()->GetPrimaryCamera();

                glm::vec4 backgroundColor = glm::vec4(0.0f);

                if (primaryCamera)
                    backgroundColor = primaryCamera.GetComponent<CameraComponent>().BackgroundColor;

                m_GameSceneRenderer->SetClearColor(backgroundColor);

                SceneManager::GetCurrentScene()->Update();
                SceneManager::GetCurrentScene()->OnRender(m_GameSceneRenderer);

                m_SceneView.SetRenderTextureID(m_GameSceneRenderer->GetFramebuffer()->GetColorAttachmentID());

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
        m_SceneView.OnEvent(event);
        m_SHierarchy.OnEvent(event);

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
                    m_SHierarchy.SetOpen(true);

                if (ImGui::MenuItem("Editor View"))
                    m_SceneView.SetOpen(true);

                if (ImGui::MenuItem("Performance"))
                    m_PerformanceOpen = true;

                if (ImGui::MenuItem("Renderer Stats"))
                    m_RendererStatsOpen = true;

                if (ImGui::MenuItem("ECS Panel"))
                    m_ECSPanel.SetOpen(true);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools")) 
            {
                if (ImGui::MenuItem("Reload C# Assembly")) 
                {
                    ScriptEngine::UnloadDomain();
                    
                    CopyAssembly((m_ScriptAssemblyPath / "Temp/TerranScriptCore.dll").string(), m_ScriptAssemblyPath.string());

                    ScriptEngine::NewDomain();
                    
                    auto scriptView = SceneManager::GetCurrentScene()->GetEntitiesWith<ScriptComponent>();

                    for (auto e : scriptView)
                    {
                        Entity entity(e, SceneManager::GetCurrentScene().get());

                        ScriptEngine::InitializeScriptable(entity);
                    }
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
	}

    void EditorLayer::OnScenePlay()
    {
        // TODO: copy the current scene
        m_SceneState = SceneState::Play;
        SceneManager::SetCurrentScene(Scene::CopyScene(m_EditorScene));
        SceneManager::GetCurrentScene()->OnResize(m_SceneView.GetViewportSize().x, m_SceneView.GetViewportSize().y);

        m_SHierarchy.SetScene(SceneManager::GetCurrentScene());
        m_ECSPanel.SetContext(SceneManager::GetCurrentScene());

        TR_TRACE(SceneManager::GetCurrentScene().get());

        SceneManager::GetCurrentScene()->StartRuntime();
    }

    void EditorLayer::OnSceneStop()
    {
        m_SceneState = SceneState::Edit;
        SceneManager::GetCurrentScene()->StopRuntime();
        SceneManager::SetCurrentScene(m_EditorScene);
        m_SHierarchy.SetScene(SceneManager::GetCurrentScene());
        m_ECSPanel.SetContext(SceneManager::GetCurrentScene());
    }

	void EditorLayer::ImGuiRender()
	{
        // NOTE : this is a temporary fix
        // move the ImGui layer from the engine to the editor
        ImGuizmo::BeginFrame();

        ShowDockspace();

        ImGui::ShowDemoWindow();

        // NOTE: Make an editor setting for the selected window
        m_SHierarchy.ImGuiRender();
        m_Selected = m_SHierarchy.GetSelected();

        m_SceneView.SetSceneState(m_SceneState);

        m_SceneView.ImGuiRender(m_Selected, m_EditorCamera, [&](const char* filePath, glm::vec2 viewportSize) 
        {
            OpenScene(filePath, viewportSize);
        });

        m_PropertiesPanel.ImGuiRender(m_Selected);

        m_ContentPanel.ImGuiRender();

        m_ECSPanel.ImGuiRender();

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
            SceneSerializer sSerializer(SceneManager::GetCurrentScene());
            sSerializer.SerializeJson(scenePath);

            m_CurrentScenePath = scenePath;
        }
    }

    void EditorLayer::NewScene()
    {
        SceneManager::SetCurrentScene(CreateShared<Scene>());
        CameraComponent& cameraComponent = SceneManager::GetCurrentScene()->CreateEntity("Camera").AddComponent<CameraComponent>();

        // TODO: should add an on component added function
        cameraComponent.Camera.SetViewport(m_SceneView.GetViewportSize().x, m_SceneView.GetViewportSize().y);
        m_SHierarchy.SetScene(SceneManager::GetCurrentScene());
        m_ECSPanel.SetContext(SceneManager::GetCurrentScene());
    }

    void EditorLayer::OpenScene()
    {
        std::filesystem::path scenePath = FileUtils::OpenFile("Terran Scene\0*.terran\0");
        
        OpenScene(scenePath, m_SceneView.GetViewportSize());
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
                Shared<Scene> newScene = CreateShared<Scene>();
                SceneSerializer sSerializer(newScene);
                if (sSerializer.DesirializeJson(jsonData))
                {
                    SceneManager::SetCurrentScene(newScene);
                    SceneManager::GetCurrentScene()->OnResize(viewportSize.x, viewportSize.y);

                    m_SHierarchy.SetScene(SceneManager::GetCurrentScene());
                    m_ECSPanel.SetContext(SceneManager::GetCurrentScene());

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

    void CopyAssembly(const std::filesystem::path& source, const std::filesystem::path& destination)
    {
        if (std::filesystem::exists(source)) 
        {
            if (std::filesystem::exists(destination / source.filename()))
                std::filesystem::remove(destination / source.filename());

            std::error_code errCode;
            std::filesystem::copy(source, destination, std::filesystem::copy_options::overwrite_existing, errCode);
            
            TR_TRACE(errCode.message());
        }
            
    }
}
