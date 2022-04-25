#pragma once

#include "Terran.h"

#include "EditorPanels/SceneHierarchyPanel.h"
#include "EditorPanels/PropertiesPanel.h"
#include "EditorPanels/SceneViewPanel.h"
#include "EditorPanels/GameViewPanel.h"
#include "EditorPanels/ContentPanel.h"
#include "EditorPanels/ECSPanel.h"
#include "EditorPanels/LogPanel.h"

#include "EditorCamera.h"

#include "SceneState.h"

namespace TerranEditor
{
	using namespace TerranEngine;

	class EditorLayer : public Layer 
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnDettach() override;

		void Update(Time& time) override;

		void OnEvent(Event& event) override;
		void ImGuiRender() override;

		SceneState GetSceneState() { return m_SceneState; }

		static EditorLayer* GetInstace() { return s_Instance; }

	private:
		bool OnKeyPressedEvent(KeyPressedEvent& kEvent);

		void SaveSceneAs();
		void NewScene();

		void OpenScene();
		void OpenScene(const std::filesystem::path& scenePath, const glm::vec2& viewportSize);
		
		void SaveScene();

		void ShowDockspace();

		void OnScenePlay();
		void OnSceneStop();

		void OnSelectedChanged(Entity newSelected);
		void OnViewportSizeChanged(glm::vec2 newViewportSize);

		Entity m_RenderableEntity, m_Entity1, m_Entity2;

		EditorCamera m_EditorCamera;

		OrthographicCamera m_Camera;

		Entity m_Selected = {};
		Entity m_EditModeSelected = {};

		// ***** Panels *****
		SceneHierarchyPanel m_SceneHierarchyPanel;
		PropertiesPanel m_PropertiesPanel;
		ContentPanel m_ContentPanel;
		SceneViewPanel m_SceneViewPanel;
		ECSPanel m_ECSPanel;
		LogPanel m_LogPanel;
		//GameView m_GameView;

		// ******************
		
		Shared<Scene> m_EditorScene;
		Shared<Scene> m_ActiveScene;

		Shared<SceneRenderer> m_RuntimeSceneRenderer;
		Shared<SceneRenderer> m_EditorSceneRenderer;

		bool m_PerformanceOpen = true;
		bool m_RendererStatsOpen = true;

		bool m_ShowColliders = false;

		float m_Frametime = 0.0f;
		SceneState m_SceneState = SceneState::Edit;

		std::filesystem::path m_CurrentScenePath;
		const std::filesystem::path m_ResPath = "Resources";
		std::filesystem::path m_ScriptAssemblyPath;

		glm::vec2 m_ViewportSize = { 1280.0f, 780.0f };

		static EditorLayer* s_Instance;
	};
}