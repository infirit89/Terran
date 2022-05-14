#pragma once

#include "EditorCamera.h"

#include "Core/Base.h"

#include "Events/KeyboardEvent.h"

#include "Scene/Entity.h"

#include "Graphics/Framebuffer.h"

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

	class SceneViewPanel
	{
		using OpenSceneFN = std::function<void(const char*, glm::vec2)>;
	public:
		SceneViewPanel() = default;
		~SceneViewPanel() = default;

		void ImGuiRender(Entity selectedEntity, EditorCamera& editorCamera);

		void SetFramebuffer(const Shared<Framebuffer>& framebuffer) { m_Framebuffer = framebuffer; }

		bool IsVisible() { return m_Visible; }

		void SetOpen(bool open) { m_Open = open; }

		void OnEvent(Event& event);

		void SetOpenSceneCallback(OpenSceneFN openSceneCallback) { m_OpenSceneCallback = openSceneCallback; }
		void SetViewportSizeChangedCallback(std::function<void(glm::vec2)> callback) { m_ViewportSizeChangedCallback = callback; }
		void SetSelectedChangedCallback(std::function<void(Entity)> callback) { m_SelectedChangedCallback = callback; }

	private:
		bool OnKeyPressed(KeyPressedEvent& e);

		bool m_Open = true;
		glm::vec2 m_ViewportSize = { 1080.0f, 790.0f };
		Shared<Framebuffer> m_Framebuffer;

		GizmoType m_GizmoType = GizmoType::None;
		int m_GizmoMode = 1;

		glm::vec2 m_Position = { 0.0f, 0.0f };

		bool m_WindowMoved = false;
		bool m_Visible = false;

		bool m_UseSnapping = false;
		glm::vec3 m_Snap = { 2.0f, 2.0f, 2.0f };

		OpenSceneFN m_OpenSceneCallback;
		std::function<void(glm::vec2)> m_ViewportSizeChangedCallback;
		std::function<void(Entity)> m_SelectedChangedCallback;
	};
}