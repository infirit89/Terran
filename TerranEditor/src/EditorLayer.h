#pragma once

#include "Terran.h"

#include "EditorPanels/SceneHierarchy.h"
#include "EditorPanels/PropertiesPanel.h"
#include "EditorPanels/SceneView.h"
#include "EditorPanels/GameView.h"
#include "EditorPanels/ContentPanel.h"

#include "EditorCamera.h"

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

	private:
		bool OnKeyPressedEvent(KeyPressedEvent& kEvent);

		void SaveSceneAs();
		void NewScene();

		void OpenScene();
		
		void SaveScene();

		void ShowDockspace();

		Entity m_RenderableEntity, m_Entity1, m_Entity2;

		EditorCamera m_EditorCamera;

		OrthographicCamera m_Camera;

		Entity m_Selected = {};

		// ***** Panels *****
		SceneHierarchy m_SHierarchy;
		PropertiesPanel m_PropertiesPanel;
		ContentPanel m_ContentPanel;
		SceneView m_SceneView;
		GameView m_GameView;

		// ******************

		//Unique<Framebuffer> m_SceneViewFramebuffer;
		//Unique<Framebuffer> m_GameViewFramebuffer;

		std::string m_CurrentScenePath = std::string();

		Shared<SceneRenderer> m_GameSceneRenderer;
		Shared<SceneRenderer> m_EditorSceneRenderer;

		const std::filesystem::path m_ResPath = "Resources";

		float m_Frametime = 0.0f;
		int m_FPS = 0;
	};
}