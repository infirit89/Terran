#include "UI.h"

#include "Terran.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace TerranEditor 
{
    void UI::DrawColor4Control(const char* label, glm::vec4& value, float columnWidth)
    {
        ImGui::PushID(label);

        ImGui::Columns(2, NULL, false);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label);

        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth());

        ImVec2 buttonSize = ImVec2{ ImGui::GetContentRegionAvailWidth(), 0.0f };

        if (ImGui::ColorButton("##colbutton", ImVec4{ value.r, value.g, value.b, value.a }, 0, buttonSize))
            ImGui::OpenPopup("picker");

        if (ImGui::BeginPopup("picker"))
        {

            ImGui::ColorPicker4("##colpicker", glm::value_ptr(value));
            ImGui::EndPopup();
        }

        ImGui::Columns(1);
        ImGui::PopID();

    }

    bool UI::DrawVec2Control(const char* label, glm::vec2& value, float power, const char* format, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label);

        ImVec2 cursorPos;

        ImGuiIO io = ImGui::GetIO();

        ImGui::Columns(2, NULL, false);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label);

        ImGui::NextColumn();
        ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 2, GImGui->Style.ItemSpacing.y });

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
        if (ImGui::DragFloat("##DRX", &value.x, power, 0.0f, 0.0f, format))
            changed = true;

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
        if (ImGui::DragFloat("##DRY", &value.y, power, 0.0f, 0.0f, format))
            changed = true;

        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

        ImGui::PopItemWidth();
        ImGui::PopStyleColor(3);

        ImGui::PopStyleVar();

        ImGui::Columns(1);
        ImGui::PopID();

        return changed;
    }

    bool UI::DrawVec3Control(const char* label, glm::vec3& value, float power, const char* format, float columnWidth)
	{
        bool changed = false;

        ImGui::PushID(label);

        ImVec2 cursorPos;

        ImGuiIO io = ImGui::GetIO();

        ImGui::Columns(2, NULL, false);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label);

        ImGui::NextColumn();
        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 2, GImGui->Style.ItemSpacing.y });

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
        if (ImGui::DragFloat("##DRX", &value.x, power, 0.0f, 0.0f, format))
            changed = true;

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
        if (ImGui::DragFloat("##DRY", &value.y, power, 0.0f, 0.0f, format))
            changed = true;

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
        if (ImGui::DragFloat("##DRZ", &value.z, power, 0.0f, 0.0f, format))
            changed = true;

        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

        ImGui::PopItemWidth();
        ImGui::PopStyleVar();
        
        ImGui::Columns(1);
        ImGui::PopID();

        return changed;
	}

    bool UI::DrawFloatControl(const char* label, float& value, float power, const char* format, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label);

        ImGui::Columns(2, NULL, false);

        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label);
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth());

        if (ImGui::DragFloat("##val", &value, power, 0.0f, 0.0f, format))
            changed = true;

        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

        ImGui::Columns(1);

        ImGui::PopID();

        return changed;
    }

    bool UI::DrawIntControl(const char* label, int& value, float power, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label);

        ImGui::Columns(2, NULL, false);

        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label);
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth());

        if (ImGui::DragInt("##val", &value, power, 0.0f, 0.0f))
            changed = true;

        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

        ImGui::Columns(1);

        ImGui::PopID();

        return changed;
    }

    bool UI::DrawBoolControl(const char* label, bool& value, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label);

        ImGui::Columns(2, NULL, false);

        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label);
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth());

        changed = ImGui::Checkbox("##val", &value);

        ImGui::Columns(1);

        ImGui::PopID();

        return changed;
    }

    bool UI::DrawStringControl(const char* label, std::string& value, ImGuiInputTextFlags flags, int maxBufSize, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label);

        ImGui::Columns(2, NULL, false);

        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label);
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth());

        char* buf = new char[maxBufSize];

        memset(buf, 0, maxBufSize);

        strcpy_s(buf, maxBufSize, value.c_str());

        if (ImGui::InputText("##val", buf, maxBufSize, flags))
        {
            value = buf;
            changed = true;
        }

        ImGui::Columns(1);
        ImGui::PopID();

        return changed;
    }

    bool UI::DrawScalar(const char* label, ImGuiDataType type, void* value, float power, const char* format, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label);

        ImGui::Columns(2, NULL, false);

        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label);
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth());

        float min = 0.0f;
        float max = 0.0f;

        if (ImGui::DragScalar("##val", type, value, power, &min, &max, format, NULL))
            changed = true;

        if (ImGui::IsItemHovered() || ImGui::IsItemActive())
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

        ImGui::Columns(1);
        ImGui::PopID();

        return changed;
    }
}

