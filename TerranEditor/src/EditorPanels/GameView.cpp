#include "GameView.h"

#include "Terran.h"

#include <ImGui/imgui.h>

namespace TerranEditor 
{
    using namespace TerranEngine;
	void GameView::ImGuiRender()
	{
        if (m_Open)
        {
            ImGui::Begin("Game view", &m_Open);
            
            ImVec2 regionAvail = ImGui::GetContentRegionAvail();

            m_ViewportSize = { regionAvail.x, regionAvail.y };

            bool isFocused = ImGui::IsWindowFocused();
            bool isHovered = ImGui::IsWindowHovered();

            Application::Get()->GetImGuiLayer().SetBlockInput(!isFocused || !isHovered);

            ImGui::Image((void*)m_RenderTextureID, regionAvail, { 0, 1 }, { 1, 0 });

            m_Visible = ImGui::IsItemVisible();

            if (m_Temp && m_Visible)
            {
                m_Visible = false;
                m_Temp = false;
            }

            ImGui::End();
        }
	}
}
