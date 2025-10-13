#pragma once

#include "EditorPanels/ContentBrowserItem.h"
#include "EditorPanels/PanelManager.h"
#include "LibCore/Layer.h"
#include "Terran.h"

#include "EditorCamera.h"

#include "SceneState.h"

#include <string>
#include <filesystem>

namespace TerranEditor {

using namespace TerranEngine;

class EditorLayer : public Terran::Core::Layer {
public:
    EditorLayer(std::string const& projectPath);

    void OnAttach() override;
    void OnDetach() override;

    void Update(Terran::Core::Time& time) override;

    void OnEvent(Terran::Core::Event& event) override;
    void ImGuiRender() override;

    SceneState GetSceneState() { return m_SceneState; }

    static EditorLayer* GetInstace() { return s_Instance; }
    EditorCamera& GetEditorCamera() { return m_EditorCamera; }

    Terran::Core::Shared<PanelManager> GetPanelManager() { return m_PanelManager; }

private:
    void OpenProject(std::filesystem::path const& projectPath);

    bool OnKeyPressedEvent(KeyPressedEvent& kEvent);
    bool OnWindowCloseEvent(WindowCloseEvent& wEvent);
    bool OnGamepadConnectedEvent(GamePadConnectedEvent& gEvent);
    bool OnGamepadDisconnectedEvent(GamePadDisconnectedEvent& gEvent);

    void SaveSceneAs();
    void NewScene();

    void OpenScene();
    void OpenSceneFromAssetPath(AssetInfo const& assetInfo, glm::vec2 const& viewportSize);

    void SaveScene();

    void RenderDockspace();

    void OnScenePlay();
    void OnSceneStop();

    bool OnScriptEngineLog(ScriptEngineLogEvent& event);

    void OnViewportSizeChanged(glm::vec2 newViewportSize);
    bool OnSceneTransition(SceneTransitionEvent& sceneTransitionEvent);
    void OnContentBrowserItemClicked(Terran::Core::Shared<ContentBrowserItem> const& item);

private:
    EditorCamera m_EditorCamera;

    OrthographicCamera m_Camera;

    // ***** Panels *****
    Terran::Core::Shared<PanelManager> m_PanelManager;
    // ******************

    Terran::Core::Shared<Scene> m_EditorScene;
    // Terran::Core::Shared<Scene> m_ActiveScene;

    // Terran::Core::Shared<SceneRenderer> m_RuntimeSceneRenderer;
    Terran::Core::Shared<SceneRenderer> m_EditorSceneRenderer;

    bool m_PerformancePanelOpen = true;
    bool m_RendererStatsPanelOpen = true;
    bool m_PhysicsMaterial2DAssetEditorOpen = false;

    bool m_ShowColliders = false;

    float m_Frametime = 0.0f;
    SceneState m_SceneState = SceneState::Edit;

    std::filesystem::path m_CurrentScenePath;
    std::filesystem::path const m_ResPath = "Resources";
    std::filesystem::path m_ScriptAssemblyPath;
    std::filesystem::path m_ProjectPath;

    glm::vec2 m_ViewportSize = { 1280.0f, 780.0f };

    static EditorLayer* s_Instance;
};

}
