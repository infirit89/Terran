#pragma once

#include <glm/glm.hpp>

#include <stdint.h>

namespace TerranEditor 
{
	class GameViewPanel 
	{
	public:
		GameViewPanel() = default;
		~GameViewPanel() = default;

		void ImGuiRender();

		void SetOpen(bool open) { m_Open = open; }

		bool IsVisible() { return m_Visible; }

		glm::vec2 GetViewportSize() { return m_ViewportSize; }

		void SetRenderTextureID(uint32_t textureID) { m_RenderTextureID = textureID; }
	private:
		bool m_Open = true;
		bool m_Visible = false;

		bool m_Temp = true;
		glm::vec2 m_ViewportSize = { 1080.0f, 790.0f };
		uint32_t m_RenderTextureID;
	};
}