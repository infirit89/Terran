#pragma once

#include "Terran.h"

#include "EditorPanels/SceneHierarchy.h"
#include "EditorPanels/PropertiesPanel.h"

namespace TerranEngine
{
	class EditorLayer : public Layer 
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnDettach() override;

		void Update(float& time) override;

		void OnEvent(Event& event) override;
		void ImGuiRender() override;

	private:
		bool OnKeyPressedEvent(KeyPressedEvent& kEvent);

		void SaveSceneAs();
		void NewScene();
		void OpenScene();
		void SaveScene();

		void ShowDockspace();

		Shared<Scene> m_Scene;
		Unique<BatchRenderer2D> m_Renderer;


		OrthographicCamera m_Camera;

		Entity m_Selected = {};

		// ***** Panels *****
		SceneHierarchy m_SHierarchy;
		PropertiesPanel m_PropertiesPanel;
		// ******************
		

		glm::vec2 m_ViewportSize = { 1080.0f, 790.0f };

		std::string m_CurrentScenePath = std::string();

		float m_Frametime;
	};
}