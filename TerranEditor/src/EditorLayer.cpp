#include "EditorLayer.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "EditorUI/TerranEditorUI.h"

namespace TerranEngine
{
	EditorLayer::EditorLayer()
		: Layer("Editor"),
        m_Camera(Application::Get()->GetWindow().GetWidth() * 0.1f, Application::Get()->GetWindow().GetHeight() * 0.1f)
	{
        m_Renderer = CreateUnique<BatchRenderer2D>(20000);
        m_Renderer->CreateFramebuffer(1280, 790, false);

        m_Scene = CreateShared<Scene>();

        auto e = m_Scene->CreateEntity("Test Entity");
        e.AddComponent<SpriteRendererComponent>();

        auto camera = m_Scene->CreateEntity("Camera");
        camera.AddComponent<CameraComponent>().Camera = m_Camera;

        m_SHierarchy = SceneHierarchy(m_Scene);

	}

	void EditorLayer::OnAttach()
	{
        ImGuiIO& io = ImGui::GetIO();

        ImFontConfig config;

        io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/Roboto/Roboto-Regular.ttf", 15.0f, &config);

        config.MergeMode = true;

        io.Fonts->Build();
	}

	void EditorLayer::OnDettach()
	{
	}

	void EditorLayer::Update(float& time)
	{
        if (m_ViewportSize.x != m_Renderer->GetFramebuffer()->Width ||
            m_ViewportSize.y != m_Renderer->GetFramebuffer()->Height)
        {
           m_Renderer->GetFramebuffer()->Resize(m_ViewportSize.x, m_ViewportSize.y);
           m_Scene->OnResize(m_ViewportSize.x, m_ViewportSize.y);
        }

		RenderCommand::Clear();

        m_Renderer->ResetStats();

        m_Renderer->GetFramebuffer()->Bind();
        RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        RenderCommand::Clear();

        m_Scene->Update();

        m_Renderer->GetFramebuffer()->Unbind();

        int frames = 1 / time;

        m_Frametime = time * 1000.0f;
	}


	void EditorLayer::OnEvent(Event& event)
	{
	}

	void EditorLayer::ShowDockspace() 
	{
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        static bool p_open = true;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Terran Editor", &p_open, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
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
            if (ImGui::BeginMenu("Options"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                ImGui::Separator();

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

        ImGui::ShowDemoWindow();

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;
        
        m_SHierarchy.ImGuiRender();
        m_PropertiesPanel.ImGuiRender(m_SHierarchy.GetSelected());

        //{
        //    ImGui::Begin("Properties");
        //    //float heightFromTop = 17.6f;

        //    if (m_Selected)
        //    {
        //        if (m_Selected.HasComponent<TransformComponent>())
        //        {
        //            auto& transformComp = m_Selected.GetComponent<TransformComponent>();

        //            TerranEditorUI::DrawVec3Control("Position", transformComp.Position);
        //        }
        //    }

        //    ImGui::End();
        //}
	}
}
