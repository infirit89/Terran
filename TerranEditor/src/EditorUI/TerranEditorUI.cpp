#include "TerranEditorUI.h"


#include "Terran.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace TerranEngine 
{
    void TerranEditorUI::DrawVec3Control(const char* label, glm::vec3& value, float power, const char* format, float columnWidth)
	{
        ImGui::PushID(label);

        ImVec2 cursorPos;

        ImGuiIO io = ImGui::GetIO();

        ImGui::Columns(2, NULL, false);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label);

        ImGui::NextColumn();
        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 2, 0 });

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::Button("X", buttonSize);
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) 
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            value.x += io.MouseDelta.x * power;
        }

        ImGui::SameLine();

        cursorPos = ImGui::GetCursorPos();
        ImGui::SetCursorPosX(cursorPos.x - 4.5f);
        ImGui::DragFloat("##DRX", &value.x, power, 0.0f, 0.0f, format);
        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::Button("Y", buttonSize);
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) 
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            value.y += io.MouseDelta.x * power;
        }

        ImGui::SameLine();

        cursorPos = ImGui::GetCursorPos();
        ImGui::SetCursorPosX(cursorPos.x - 4.5f);
        ImGui::DragFloat("##DRY", &value.y, power, 0.0f, 0.0f, format);
        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::Button("Z", buttonSize);
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) 
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
            value.z += io.MouseDelta.x * power;
        }

        ImGui::PopStyleColor(3);
        ImGui::SameLine();

        cursorPos = ImGui::GetCursorPos();
        ImGui::SetCursorPosX(cursorPos.x - 4.5f);
        ImGui::DragFloat("##DRZ", &value.z, power, 0.0f, 0.0f, format);
        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

        ImGui::PopItemWidth();
        ImGui::PopStyleVar();
        
        ImGui::Columns(1);
        ImGui::PopID();
	}
}

