#pragma once

#include "Terran.h"
#include "../EditorCamera.h"

namespace TerranEditor 
{
	using namespace TerranEngine;
	class SceneView
	{
		using OpenSceneFN = std::function<void(const char* scenePath, glm::vec2 viewPortSize)>;
	public:
		SceneView() = default;
		~SceneView() = default;


		void ImGuiRender(Entity selectedEntity, EditorCamera& editorCamera, OpenSceneFN openSceneFN);

		glm::vec2 GetViewportSize() { return m_ViewportSize; }

		void SetRenderTextureID(uint32_t textureID) { m_RenderTextureID = textureID; }

		bool IsVisible() { return m_Visible; }

		void SetOpen(bool open) { m_Open = open; }

		void OnEvent(Event& event);

	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		bool m_Open = true;
		glm::vec2 m_ViewportSize = { 1080.0f, 790.0f };
		uint32_t m_RenderTextureID = -1;

		int m_GizmoType = 7;

		glm::vec2 m_Position = { 0.0f, 0.0f };

		bool m_WindowMoved = false;
		bool m_Visible = false;

	};
}