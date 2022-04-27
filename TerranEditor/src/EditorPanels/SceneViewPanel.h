#pragma once

#include "Terran.h"

#include "EditorCamera.h"

namespace TerranEditor 
{
	using namespace TerranEngine;

	class SceneViewPanel
	{
		using OpenSceneFN = std::function<void(const char* filePath, glm::vec2 viewportSize)>;
	public:
		SceneViewPanel() = default;
		~SceneViewPanel() = default;

		void ImGuiRender(Entity selectedEntity, EditorCamera& editorCamera);

		void SetRenderTextureID(uint32_t textureID) { m_RenderTextureID = textureID; }

		bool IsVisible() { return m_Visible; }

		void SetOpen(bool open) { m_Open = open; }

		void OnEvent(Event& event);

		void SetOpenSceneCallback(OpenSceneFN openSceneCallback) { m_OpenSceneCallback = openSceneCallback; }
		void SetViewportSizeChangedCallback(std::function<void(glm::vec2 viewportSize)> callback) { m_ViewportSizeChangedCallback = callback; }

	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		bool m_Open = true;
		glm::vec2 m_ViewportSize = { 1080.0f, 790.0f };
		uint32_t m_RenderTextureID = -1;

		int m_GizmoType = 7;
		int m_GizmoMode = 1;

		glm::vec2 m_Position = { 0.0f, 0.0f };

		bool m_WindowMoved = false;
		bool m_Visible = false;

		bool m_UseSnapping = false;
		glm::vec3 m_Snap = { 2.0f, 2.0f, 2.0f };

		OpenSceneFN m_OpenSceneCallback;
		std::function<void(glm::vec2 viewportSize)> m_ViewportSizeChangedCallback;
	};
}