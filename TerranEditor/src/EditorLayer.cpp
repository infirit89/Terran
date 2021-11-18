#include "EditorLayer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "UI/TerranEditorUI.h"

namespace TerranEngine
{
	EditorLayer::EditorLayer()
		: Layer("Editor")
	{
        m_Renderer = CreateUnique<BatchRenderer2D>(20000);
        m_Renderer->CreateFramebuffer(1280, 790, false);

        m_Scene = CreateShared<Scene>();

        m_Entity1 = m_Scene->CreateEntity("Entity 1");

        m_Entity2 = m_Scene->CreateEntity("Entity");
        m_RenderableEntity = m_Scene->CreateEntity("Test Entity");
        m_RenderableEntity.AddComponent<SpriteRendererComponent>();
        
        auto entity = m_Scene->CreateEntity("Bruuh");
        
        auto camera = m_Scene->CreateEntity("Camera");
        camera.AddComponent<CameraComponent>().Camera = m_Camera;


        m_SHierarchy = SceneHierarchy(m_Scene);

        auto foundEntity = m_Scene->FindEntityWithUUID(entity.GetID());
        foundEntity.AddComponent<SpriteRendererComponent>();
        //TR_TRACE(foundEntity.GetName());

	}

	void EditorLayer::OnAttach()
	{
        ImGuiIO& io = ImGui::GetIO();

        ImFontConfig config;

        io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/Roboto/Roboto-Regular.ttf", 15.0f, &config);

        config.MergeMode = true;

        io.Fonts->Build();

        io.IniFilename = "res\\TerranEditorSettings.ini";
	}

	void EditorLayer::OnDettach()
	{
	}

	void EditorLayer::Update(float& time)
	{
        if ((m_ViewportSize.x != m_Renderer->GetFramebuffer()->Width ||
            m_ViewportSize.y != m_Renderer->GetFramebuffer()->Height) && m_ViewportSize.x > 0 && m_ViewportSize.y > 0)
        {
           m_Renderer->GetFramebuffer()->Resize(m_ViewportSize.x, m_ViewportSize.y);
           m_Scene->OnResize(m_ViewportSize.x, m_ViewportSize.y);
        }
        
        RenderCommand::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		RenderCommand::Clear();

        m_Renderer->ResetStats();

        m_Renderer->GetFramebuffer()->Bind();
        auto primaryCamera = m_Scene->GetPrimaryCamera();
        glm::vec4 backgroundColor = glm::vec4(0.0f);

        if(primaryCamera)
            backgroundColor = primaryCamera.GetComponent<CameraComponent>().BackgroundColor;

        RenderCommand::SetClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
        RenderCommand::Clear();

        m_Scene->Update();

        m_Renderer->GetFramebuffer()->Unbind();

        int frames = 1 / time;

        m_Frametime = time * 1000.0f;
	}


	void EditorLayer::OnEvent(Event& event)
	{
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

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
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

            ImGui::EndMenuBar();
        }

        ImGui::End();
	}

	void EditorLayer::ImGuiRender()
	{
        ShowDockspace();

        // Viewport
        {
            ImGui::Begin("Viewport");

            ImVec2 regionAvail = ImGui::GetContentRegionAvail();

            m_ViewportSize = { regionAvail.x, regionAvail.y };

            uint32_t textureID = m_Renderer->GetFramebuffer()->GetColorAttachmentID();
            
            ImGui::Image((void*)textureID, regionAvail, { 0, 1 }, { 1, 0 });

            ImGui::End();
        }

        // Renderer stats
        {
            ImGui::Begin("Renderer Stats");

            BatchRendererStats stats = m_Renderer->GetStats();

            ImGui::Text("Draw calls: %d", stats.DrawCalls);
            ImGui::Text("Total Quad count: %d", stats.GetQuadCount());
            ImGui::Text("Total Vertex count: %d", stats.VertexCount);
            ImGui::Text("Total Index count: %d", stats.IndexCount);

            ImGui::Text("Frame Time: %f ms/frame", m_Frametime);

            ImGui::End();
        }

        m_SHierarchy.ImGuiRender();
        m_PropertiesPanel.ImGuiRender(m_SHierarchy.GetSelected());
	}

    void EditorLayer::SaveSceneAs()
    {
        std::string scenePath = FileUtils::SaveFile("Terran Scene\0*.terran\0");
        if (!scenePath.empty())
        {
            m_CurrentScenePath = scenePath;
            SceneSerializer sSerializer(m_Scene);
            sSerializer.SerializeJson(scenePath);
        }
    }

    void EditorLayer::NewScene()
    {
        m_Scene = CreateShared<Scene>();
        m_Scene->CreateEntity("Camera").AddComponent<CameraComponent>();
        m_SHierarchy.SetScene(m_Scene);
    }

    void EditorLayer::OpenScene()
    {
        std::string scenePath = FileUtils::OpenFile("Terran Scene\0*.terran\0");

        if (!scenePath.empty())
        {
            m_CurrentScenePath = scenePath;
            Shared<Scene> newScene = CreateShared<Scene>();
            SceneSerializer sSerializer(newScene);
            sSerializer.DesirializeJson(scenePath);
            m_Scene = newScene;
            m_Scene->OnResize(m_ViewportSize.x, m_ViewportSize.y);

            m_SHierarchy.SetScene(m_Scene);
        }
    }

    void EditorLayer::SaveScene()
    {
        if (m_CurrentScenePath.empty())
            SaveSceneAs();
        else 
        {
            SceneSerializer sSerializer(m_Scene);
            sSerializer.SerializeJson(m_CurrentScenePath);
        }
    }
}
