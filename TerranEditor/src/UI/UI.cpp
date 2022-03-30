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
        
        {
            ScopedVarTable::TableInfo tableInfo;
            ScopedVarTable colorControlTable(label, tableInfo);
            ImVec2 buttonSize = ImVec2{ ImGui::GetContentRegionAvailWidth(), 0.0f };

            if (ImGui::ColorButton("##colbutton", ImVec4{ value.r, value.g, value.b, value.a }, 0, buttonSize))
                ImGui::OpenPopup("picker");

            if (ImGui::BeginPopup("picker"))
            {

                ImGui::ColorPicker4("##colpicker", glm::value_ptr(value));
                ImGui::EndPopup();
            }
        }
        
        ImGui::PopID();

    }

    bool UI::DrawVec2Control(const char* label, glm::vec2& value, float power, const char* format, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label);

        ImVec2 cursorPos;

        ImGuiIO io = ImGui::GetIO();
        {
            ScopedVarTable::TableInfo tableInfo;
            tableInfo.itemCount = 2;
            ScopedVarTable vec2Table(label, tableInfo);

            ScopedStyleVar itemSpacingStyleVar({
                { ImGuiStyleVar_ItemSpacing, { 2, GImGui->Style.ItemSpacing.y } }
                });

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight, lineHeight };
        
            ScopedStyleColor buttonStyleColor({
                { ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f } },
                { ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f } },
                { ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f } }
                });

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

        }


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

        {
            ScopedVarTable::TableInfo tableInfo;
            tableInfo.itemCount = 3;
            ScopedVarTable vec3Table(label, tableInfo);

            ScopedStyleVar itemSpacingStyleVar({
               { ImGuiStyleVar_ItemSpacing, { 2, GImGui->Style.ItemSpacing.y } }
                });

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight, lineHeight };

            ScopedStyleColor buttonStyleColor({
                { ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f } },
                { ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f } },
                { ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f } }
                });

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

            ImGui::SameLine();

            cursorPos = ImGui::GetCursorPos();
            ImGui::SetCursorPosX(cursorPos.x - 4.5f);
            if (ImGui::DragFloat("##DRZ", &value.z, power, 0.0f, 0.0f, format))
                changed = true;

            if (ImGui::IsItemHovered() || ImGui::IsItemActive())
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

            ImGui::PopItemWidth();
        }
        
        ImGui::Columns(1);
        ImGui::PopID();

        return changed;
	}

    bool UI::DrawFloatControl(const char* label, float& value, float power, const char* format, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label);
        
        {
            ScopedVarTable::TableInfo tableInfo;

            ScopedVarTable floatTable(label, tableInfo);

            if (ImGui::DragFloat("##val", &value, power, 0.0f, 0.0f, format))
                changed = true;

            if (ImGui::IsItemHovered() || ImGui::IsItemActive())
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        ImGui::PopID();

        return changed;
    }

    bool UI::DrawIntControl(const char* label, int& value, float power, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label);

        {
            ScopedVarTable::TableInfo tableInfo;
            ScopedVarTable intTable(label, tableInfo);

            if (ImGui::DragInt("##val", &value, power, 0.0f, 0.0f))
                changed = true;

            if (ImGui::IsItemHovered() || ImGui::IsItemActive())
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }


        ImGui::PopID();

        return changed;
    }

    bool UI::DrawBoolControl(const char* label, bool& value, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label);

        {
            ScopedVarTable::TableInfo tableInfo;

            ScopedVarTable floatTable(label, tableInfo);
            changed = ImGui::Checkbox("##val", &value);
        }

        ImGui::PopID();

        return changed;
    }

    bool UI::DrawStringControl(const char* label, std::string& value, ImGuiInputTextFlags flags, int maxBufSize, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label);

        {
            ScopedVarTable::TableInfo tableInfo;

            ScopedVarTable floatTable(label, tableInfo);

            char* buf = new char[maxBufSize];

            memset(buf, 0, maxBufSize);

            strcpy_s(buf, maxBufSize, value.c_str());

            if (ImGui::InputText("##val", buf, maxBufSize, flags))
            {
                value = buf;
                changed = true;
            }
        }

        ImGui::PopID();

        return changed;
    }

    bool UI::DrawScalar(const char* label, ImGuiDataType type, void* value, float power, const char* format, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label);
        {
            ScopedVarTable::TableInfo tableInfo;

            ScopedVarTable floatTable(label, tableInfo);

            float min = 0.0f;
            float max = 0.0f;

            if (ImGui::DragScalar("##val", type, value, power, &min, &max, format, NULL))
                changed = true;

            if (ImGui::IsItemHovered() || ImGui::IsItemActive())
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

        }

        ImGui::PopID();

        return changed;
    }

    UI::ScopedVarTable::ScopedVarTable(const char* name, TableInfo tableInfo)
        : m_TableInfo(tableInfo)
    {
        // NOTE: this is a very fucking bad
        //char* tableID = new char[strlen(name) + 2];

        //tableID[0] = '#';
        //strcat(tableID, name);
        //tableID[strlen(name)] = '\0';

        ImGui::Columns(tableInfo.columnCount, nullptr, tableInfo.border);
        ImGui::SetColumnWidth(0, tableInfo.firstColumnWidth);
        ImGui::Text(name);
        ImGui::NextColumn();

        if (tableInfo.itemCount > 1)
            ImGui::PushMultiItemsWidths(tableInfo.itemCount, ImGui::CalcItemWidth());
        else
            ImGui::PushItemWidth(ImGui::CalcItemWidth());

        //delete[] tableID;
    }

    UI::ScopedVarTable::~ScopedVarTable()
    {
        if (m_TableInfo.itemCount == 1)
            ImGui::PopItemWidth();

        ImGui::Columns(1);
    }

    UI::ScopedStyleColor::ScopedStyleColor(std::initializer_list<StyleColor> styleColorList)
        : m_StyleColorListSize(styleColorList.size())
    {
        for (auto& styleColor : styleColorList)
        {
            ImVec4 color = { styleColor.Color.x, styleColor.Color.y, styleColor.Color.z, styleColor.Color.w };

            ImGui::PushStyleColor(styleColor.ColorVarIdx, color);
        }
    }

    UI::ScopedStyleColor::~ScopedStyleColor()
    {
        ImGui::PopStyleColor(m_StyleColorListSize);
    }

    UI::ScopedStyleVar::ScopedStyleVar(std::initializer_list<StyleVar> styleVarList)
        : m_StyleVarListSize(styleVarList.size())
    {
        for (auto& styleVarIt : styleVarList)
        {
            ImVec2 styleVal = { styleVarIt.Val.x, styleVarIt.Val.y };

            ImGui::PushStyleVar(styleVarIt.StyleVarIdx, styleVal);
        }
    }

    UI::ScopedStyleVar::~ScopedStyleVar()
    {
        ImGui::PopStyleVar(m_StyleVarListSize);
    }
}

