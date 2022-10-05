#pragma once

#include "Terran.h"
#include "EditorPanels/PanelManager.h"

#include "EditorCamera.h"

#include "SceneState.h"

namespace TerranEditor
{
	using namespace TerranEngine;

	class EditorLayer : public Layer 
	{
	public:
		EditorLayer(const std::string& projectPath);

		void OnAttach() override;
		void OnDettach() override;

		void Update(Time& time) override;

		void OnEvent(Event& event) override;
		void ImGuiRender() override;

		SceneState GetSceneState() { return m_SceneState; }

		static EditorLayer* GetInstace() { return s_Instance; }
		EditorCamera& GetEditorCamera() { return m_EditorCamera; }

	private:
        void OpenProject(const std::filesystem::path& projectPath);

		bool OnKeyPressedEvent(KeyPressedEvent& kEvent);
		bool OnWindowCloseEvent(WindowCloseEvent& wEvent);
		
		void SaveSceneAs();
		void NewScene();

		void OpenScene();
		void OpenScene(const std::filesystem::path& scenePath, const glm::vec2& viewportSize);
		
		void SaveScene();

		void ShowDockspace();

		void OnScenePlay();
		void OnSceneStop();

		void OnViewportSizeChanged(glm::vec2 newViewportSize);

		EditorCamera m_EditorCamera;

		OrthographicCamera m_Camera;

		// ***** Panels *****
		/*SceneHierarchyPanel m_SceneHierarchyPanel;
		PropertiesPanel m_PropertiesPanel;
		ContentPanel m_ContentPanel;
		SceneViewPanel m_SceneViewPanel;
		ECSPanel m_ECSPanel;
		LogPanel m_LogPanel;*/
		//GameView m_GameView;
		Shared<PanelManager> m_PanelManager;
		// ******************
		
		Shared<Scene> m_EditorScene;
		//Shared<Scene> m_ActiveScene;

		//Shared<SceneRenderer> m_RuntimeSceneRenderer;
		Shared<SceneRenderer> m_EditorSceneRenderer;

		bool m_PerformancePanelOpen = true;
		bool m_RendererStatsPanelOpen = true;

		bool m_ShowColliders = false;

		float m_Frametime = 0.0f;
		SceneState m_SceneState = SceneState::Edit;

		std::filesystem::path m_CurrentScenePath;
		const std::filesystem::path m_ResPath = "Resources";
		std::filesystem::path m_ScriptAssemblyPath;
        std::filesystem::path m_ProjectPath;

		glm::vec2 m_ViewportSize = { 1280.0f, 780.0f };

		static EditorLayer* s_Instance;
	};
}

