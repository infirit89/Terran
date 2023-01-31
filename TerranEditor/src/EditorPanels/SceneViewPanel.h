#pragma once

#include "EditorCamera.h"
#include "EditorPanel.h"

#include "Core/Base.h"

#include "Events/KeyboardEvent.h"

#include "Scene/Entity.h"
#include "Scene/Systems/SceneRenderer.h"

namespace TerranEditor 
{
	using namespace TerranEngine;

	enum class GizmoType 
	{
		None = 0,
		Translaste,
		Rotate,
		Scale
	};

	class SceneViewPanel : public EditorPanel
	{
		using OpenSceneFN = std::function<void(const std::filesystem::path&, glm::vec2)>;
	public:
		SceneViewPanel() = default;
		~SceneViewPanel() = default;

		virtual void ImGuiRender() override;

		void SetSceneRenderer(const Shared<SceneRenderer>& sceneRenderer) { m_SceneRenderer = sceneRenderer; }

		//bool IsVisible() { return m_Visible; }

		virtual void OnEvent(Event& event) override;

		void SetOpenSceneCallback(OpenSceneFN openSceneCallback) { m_OpenSceneCallback = openSceneCallback; }
		void SetViewportSizeChangedCallback(std::function<void(glm::vec2)> callback) { m_ViewportSizeChangedCallback = callback; }

		virtual void SetSceneContext(const Shared<Scene>& context) override { m_Scene = context; }
		
	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		glm::vec2 m_ViewportSize = { 1080.0f, 790.0f };
		Shared<SceneRenderer> m_SceneRenderer;

		GizmoType m_GizmoType = GizmoType::None;
		int m_GizmoMode = 1;

		glm::vec2 m_Position = { 0.0f, 0.0f };

		bool m_WindowMoved = false;
		bool m_Visible = false;

		bool m_UseSnapping = false;
		glm::vec3 m_Snap = { 2.0f, 2.0f, 2.0f };

		OpenSceneFN m_OpenSceneCallback;
		std::function<void(glm::vec2)> m_ViewportSizeChangedCallback;
	};
}