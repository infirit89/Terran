#include "UI.h"

#include "Terran.h"
#include "Core/Log.h"
#include "Physics/PhysicsLayerManager.h"

#include "EditorResources.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <IconsFontAwesome6.h>

#include <glm/gtc/type_ptr.hpp>

#include <stack>

namespace TerranEditor 
{
	using namespace TerranEngine;

	static int s_CurrentId = 0;
	static std::stack<float> s_FontSizeStack;

	static std::string ProccessFieldName(std::string name)
	{
		std::string result;

		name[0] = toupper(name[0]);

		for (size_t i = 0; i < name.size(); i++)
		{
			if (isupper(name.at(i)))
			{
				result += " ";
				result += name.at(i);
			}
			else
				result += name.at(i);
		}

		return result;
	}

	// consider moving these 2 functions to the physics layer manager
	static std::array<bool, PhysicsLayerManager::GetMaxLayerCount()> GetLayerIndicesFromMask(uint16_t layerMask) 
	{
		std::array<bool, PhysicsLayerManager::GetMaxLayerCount()> layerIndices({ false });

		for (size_t i = 0; i < PhysicsLayerManager::GetMaxLayerCount(); i++)
		{
			if (((1 << i) & layerMask) != 0)
				layerIndices[i] = true;
		}

		return layerIndices;
	}

	static uint16_t GetLayerMaskFromIndices(const std::array<bool, PhysicsLayerManager::GetMaxLayerCount()>& layerIndices) 
	{
		uint16_t layerMask = 0;
		for (size_t i = 0; i < PhysicsLayerManager::GetMaxLayerCount(); i++)
		{
			if (layerIndices[i])
				layerMask |= 1 << i;
		}

		return layerMask;
	}

	bool UI::PropertyDropdownMultipleSelect(const std::string& label, const char** stateNames, size_t stateCount, bool* selectedElements)
	{
		ImGui::PushID(label.c_str());
		bool changed = false;

		uint32_t selectedCount = 0;
		int lastSelectedIndex = -1;

		for (size_t i = 0; i < stateCount; i++)
		{
			if (selectedElements[i])
			{
				selectedCount++;
				lastSelectedIndex = (int)i;
			}
		}

		const char* currentState = "Nothing";

		if (selectedCount == 1)
			currentState = stateNames[lastSelectedIndex];
		else if(selectedCount > 1)
			currentState = "Mixed";

		UI::ScopedVarTable::TableInfo tableInfo;
		UI::ScopedVarTable comboBoxTable(label, tableInfo);

		std::string comboHash = "##" + label;
		if (ImGui::BeginCombo(comboHash.c_str(), currentState))
		{
			for (size_t i = 0; i < stateCount; i++)
			{
				if (strlen(stateNames[i]) == 0) continue;

				if (ImGui::Selectable(stateNames[i], selectedElements[i], ImGuiSelectableFlags_DontClosePopups))
				{
					selectedElements[i] = !(selectedElements[i]);
					changed = true;
				}

			}

			ImGui::EndCombo();
		}

		ImGui::PopID();
		return changed;
	}

	UI::ScopedVarTable::ScopedVarTable(const std::string& name, TableInfo tableInfo)
		: m_TableInfo(tableInfo)
	{
		// NOTE: this is a very fucking bad
		char tableID[256];

		tableID[0] = '#';
		strcpy_s(tableID + 1, sizeof(tableID) - 1, name.c_str());
		tableID[strlen(name.c_str()) + 1] = '\0';

		ImGui::Columns(tableInfo.ColumnCount, tableID, tableInfo.Border);
		ImGui::SetColumnWidth(0, tableInfo.FirstColumnWidth);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		if (tableInfo.ItemCount > 1)
			ImGui::PushMultiItemsWidths(tableInfo.ItemCount, ImGui::CalcItemWidth());
		else
			ImGui::PushItemWidth(ImGui::CalcItemWidth());

	}

	UI::ScopedVarTable::~ScopedVarTable()
	{
		if (m_TableInfo.ItemCount == 1)
			ImGui::PopItemWidth();

		ImGui::Columns(1);
	}

	UI::ScopedStyleColor::ScopedStyleColor(std::initializer_list<StyleColor> styleColorList)
		: m_StyleColorListSize(styleColorList.size())
	{
		for (auto& styleColor : styleColorList)
			ImGui::PushStyleColor(styleColor.ColorVarIdx, styleColor.Color);
	}

	UI::ScopedStyleColor::~ScopedStyleColor()
	{ 
		ImGui::PopStyleColor((int)m_StyleColorListSize);
	}

	UI::ScopedStyleVar::ScopedStyleVar(std::initializer_list<StyleVar> styleVarList)
		: m_StyleVarListSize(styleVarList.size())
	{
		for (auto& styleVarIt : styleVarList)
		{
			const ImGuiDataVarInfo* varInfo = ImGui::GetStyleVarInfo(styleVarIt.StyleVarIdx);

			if(varInfo->Count == 1)
				ImGui::PushStyleVar(styleVarIt.StyleVarIdx, (float)styleVarIt);
			else
				ImGui::PushStyleVar(styleVarIt.StyleVarIdx, (ImVec2)styleVarIt);
		}
	}

	UI::ScopedStyleVar::~ScopedStyleVar()
	{
		ImGui::PopStyleVar((int)m_StyleVarListSize);
	}

	void UI::PushID()
	{
		ImGui::PushID(s_CurrentId++);
	}

	void UI::PopID()
	{
		ImGui::PopID();
		--s_CurrentId;
	}

	int UI::GenerateID()
	{
		return ++s_CurrentId;
	}

	void UI::PushFontSize(float fontSize)
	{
		s_FontSizeStack.push(ImGui::GetFontSize());
		GImGui->FontSize = fontSize;
	}

	void UI::PopFontSize()
	{
		GImGui->FontSize = s_FontSizeStack.top();
		s_FontSizeStack.pop();
	}

	void UI::ShiftCursor(float x, float y)
	{
		ImGui::SetCursorPos({ ImGui::GetCursorPos().x  +  x, ImGui::GetCursorPos().y + y });
	}

	void UI::ShiftCursorX(float x)
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x);
	}

	void UI::ShiftCursorY(float y)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y);
	}

	static void ScaleUI() 
	{
		glm::vec2 contentScale = Application::Get()->GetWindow().GetContentScale();
		ImGuiStyle* style = &ImGui::GetStyle();
		style->ScaleAllSizes(contentScale.x);
	}

	void UI::SetupUIStyle2()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		style->WindowPadding = ImVec2(8.0f, 8.0f);
		style->WindowRounding = 5.0f;
		style->FramePadding = ImVec2(5.0f, 5.0f);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(12.0f, 8.0f);
		style->ItemInnerSpacing = ImVec2(8.0f, 6.0f);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 4.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 0.0f;
		style->TabRounding = 4.0f;
		style->ChildRounding = 0.0f;
		style->PopupRounding = 0.0f;

		style->WindowBorderSize = 1.0f;
		style->FrameBorderSize = 0.0f;
		style->PopupBorderSize = 1.0f;

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.27f, 0.27f, 0.29f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.32f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.42f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.53f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.47f, 0.39f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.44f, 0.44f, 0.47f, 0.59f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.20f, 0.22f, 0.39f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.44f, 0.44f, 0.47f, 0.39f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.91f, 0.62f, 0.00f, 0.78f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.91f, 0.62f, 0.00f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		ScaleUI();
	}

	void UI::SetupUIStyle3()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(5.00f, 2.00f);
		style.CellPadding = ImVec2(6.00f, 6.00f);
		style.ItemSpacing = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 25;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 10;
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 1;
		style.TabBorderSize = 1;
		style.WindowRounding = 7;
		style.ChildRounding = 4;
		style.FrameRounding = 3;
		style.PopupRounding = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding = 4;

		ScaleUI();
	}

	void UI::SetupUIStyle4()
	{
		// Fork of Deep Dark style from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.4f;

		// Rounding
		style.WindowRounding = 7.0f;
		style.ChildRounding = 4.0f;
		style.PopupRounding = 6.0f;
		style.FrameRounding = 3.0f;
		style.TabRounding = 4.0f;
		style.GrabRounding = 3.0f;
		style.ScrollbarRounding = 9.0f;

		// Padding
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.FramePadding = ImVec2(5.0f, 2.0f);
		style.CellPadding = ImVec2(6.0f, 6.0f);

		// Spacing
		style.ItemSpacing = ImVec2(6.0f, 6.0f);
		style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
		style.IndentSpacing = 16.0f;
		style.ColumnsMinSpacing = 6.0f;

		// Size
		style.ChildBorderSize = 1.0f;
		style.PopupBorderSize = 1.0f;
		style.FrameBorderSize = 1.0f;
		style.ScrollbarSize = 15.0f;
		style.GrabMinSize = 10.0f;
		style.TabBorderSize = 1.0f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = ImVec2(32.0f, 32.0f);

		// Alignment
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.63f, 0.63f, 0.63f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.0f);
		colors[ImGuiCol_Border] = ImVec4(0.18f, 0.18f, 0.18f, 0.28f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.23f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.18f, 0.68f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.18f, 0.18f, 0.18f, 0.27f);
		colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.37f, 0.37f, 0.37f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.37f, 0.37f, 0.37f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.37f, 0.37f, 0.37f, 0.35f);

		ScaleUI();
	}

	void UI::Image(const Shared<Texture>& texture, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec4& color)
	{
		ImGui::Image(reinterpret_cast<ImTextureID>((uint64_t)texture->GetHandle()),
					{ size.x, size.y },
					{ uv0.x, uv0.y },
					{ uv1.x, uv1.y },
					{ color.r, color.g, color.b, color.a });
	}

	bool UI::SearchInput(ImGuiTextFilter& filter, const std::string& hint, float width)
	{
		UI::PushID();
		const float frameBorderSize = ImGui::GetStyle().FrameBorderSize;
		const float framePaddingX = ImGui::GetStyle().FramePadding.x;
		UI::ScopedStyleVar frameBorder({
			{ ImGuiStyleVar_FrameBorderSize, frameBorderSize },
			{ ImGuiStyleVar_FramePadding, { framePaddingX, 5.0f } }
		});

		const float framePaddingY = ImGui::GetStyle().FramePadding.y;
		const float cursorPos = ImGui::GetCursorPosX();
		bool modified = false;

		ImGui::SetNextItemWidth(width);
		ImGui::SetNextItemInnerWidth(width - (ImGui::GetFontSize() + 12.0f));

		ImGui::SetNextItemAllowOverlap();
		if (ImGui::InputText("", filter.InputBuf, IM_ARRAYSIZE(filter.InputBuf)))
			modified = true;
		else if(ImGui::IsItemDeactivatedAfterEdit())
			modified = true;

		bool searching = filter.InputBuf[0] != 0;

		ImGui::SameLine(cursorPos + 5.0f);
		ImGui::BeginHorizontal("", ImGui::GetItemRectSize());
		const glm::vec2 iconSize = { ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight() };

		if (!searching)
		{
			UI::ScopedStyleColor textColor({
				{ ImGuiCol_Text, { ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled) } }
			});

			{
				UI::ScopedFont scopedIconFont("IconFont");
				//const float originalFontSize = ImGui::GetFontSize();
				constexpr float iconSize = 12.0f;
				UI::PushFontSize(iconSize);
				const float iconOffset = framePaddingY - 3.0f;
				UI::ShiftCursorY(iconOffset);
				ImGui::TextUnformatted(ICON_FA_MAGNIFYING_GLASS);
				UI::PopFontSize();
				UI::ShiftCursorY(-iconOffset);
			}

			UI::ScopedFont defaultFont("Roboto-Regular");
			UI::ShiftCursorX(-1.0f);
			ImGui::TextUnformatted(hint.c_str());
			UI::ShiftCursorX(1.0f);
		}
		else 
		{
			UI::ScopedStyleColor clearButtonColor({
				{ ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f } }
			});

			UI::ScopedStyleVar clearButtonStyle({
				{ ImGuiStyleVar_FrameBorderSize, 0.0f }
			});

			UI::ScopedFont scopedIconFont("IconFont");

			const float lineHeight = ImGui::GetItemRectSize().y - framePaddingY / 2.0f;

			ImGui::Spring();
			
			if (ImGui::ButtonEx(ICON_FA_XMARK, { lineHeight, lineHeight }))
			{
				memset(filter.InputBuf, 0, IM_ARRAYSIZE(filter.InputBuf));
				modified = true;
			}

			if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) 
				ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

			ImGui::Spring(-1.0f, 4.0f * 2.0f);
		}

		ImGui::EndHorizontal();
		UI::PopID();
		return modified;
	}

	void UI::Tooltip(const char* text)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(text);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	void UI::SetupUIStyle()
	{
		// Gruvbox style from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.0;
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.WindowRounding = 0.0f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.ChildRounding = 0.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 0.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(3.9f, 3.0f);
		style.FrameRounding = 0.0f;
		style.FrameBorderSize = 0.0f;
		style.ItemSpacing = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
		style.CellPadding = ImVec2(4.0f, 2.0f);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 6.0f;
		style.ScrollbarSize = 14.0f;
		style.ScrollbarRounding = 0.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 0.0f;
		style.TabRounding = 0.0f;
		style.TabBorderSize = 0.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text] = ImVec4(0.9742489457130432f, 0.9573628306388855f, 0.8864410519599915f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.658823549747467f, 0.6000000238418579f, 0.5176470875740051f, 1.0f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 1.0f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.8f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9215686321258545f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.5882353186607361f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.5882353186607361f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.7843137383460999f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.9803921580314636f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1176470592617989f, 0.1098039224743843f, 0.105882354080677f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.03529411926865578f, 0.0470588244497776f, 0.05490196123719215f, 1.0f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1568627506494522f, 0.1411764770746231f, 0.1333333402872086f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.4000000059604645f, 0.3607843220233917f, 0.3294117748737335f, 0.5882353186607361f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.9215686321258545f, 0.8588235378265381f, 0.6980392336845398f, 0.5882353186607361f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.9215686321258545f, 0.8588235378265381f, 0.6980392336845398f, 0.7843137383460999f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.9215686321258545f, 0.8588235378265381f, 0.6980392336845398f, 0.8823529481887817f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.7450980544090271f, 0.1176470592617989f, 0.0784313753247261f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.7450980544090271f, 0.1176470592617989f, 0.0784313753247261f, 1.0f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8235294222831726f, 0.196078434586525f, 0.1568627506494522f, 1.0f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.4901960790157318f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.686274528503418f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.8823529481887817f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.3921568691730499f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.5882353186607361f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.321568638086319f, 0.2823529541492462f, 0.250980406999588f, 0.7843137383460999f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 0.6000000238418579f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.6000000238418579f, 0.6000000238418579f, 0.6980392336845398f, 1.0f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.6980392336845398f, 0.6980392336845398f, 0.8980392217636108f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.9215686321258545f, 0.8588235378265381f, 0.6980392336845398f, 1.0f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.7764706015586853f, 0.8196078538894653f, 1.0f, 0.6000000238418579f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.7764706015586853f, 0.8196078538894653f, 1.0f, 0.8999999761581421f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.4313725531101227f, 0.105882354080677f, 0.062745101749897f, 0.8823529481887817f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.5490196347236633f, 0.105882354080677f, 0.062745101749897f, 1.0f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.5882353186607361f, 0.105882354080677f, 0.062745101749897f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.4313725531101227f, 0.105882354080677f, 0.062745101749897f, 0.8823529481887817f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.5882353186607361f, 0.105882354080677f, 0.062745101749897f, 0.7843137383460999f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.9843137264251709f, 0.9450980424880981f, 0.7803921699523926f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.2705882489681244f, 0.5215686559677124f, 0.5333333611488342f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.843137264251709f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.2705882489681244f, 0.5215686559677124f, 0.5333333611488342f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.407843142747879f, 0.3568627536296844f, 0.3137255012989044f, 1.0f);
		style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.4941176474094391f, 0.43529412150383f, 0.3764705955982208f, 1.0f);
		style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.1568627506494522f, 0.1411764770746231f, 0.1333333402872086f, 0.501960813999176f);
		style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.1568627506494522f, 0.1411764770746231f, 0.1333333402872086f, 0.501960813999176f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.02745098061859608f, 0.4000000059604645f, 0.4705882370471954f, 1.0f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.4470588266849518f, 0.4470588266849518f, 0.8980392217636108f, 0.800000011920929f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7019608020782471f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.3499999940395355f);

		ScaleUI();
	}
	bool UI::BeginPropertyGroup(const char* propertyGroupName)
	{
		if (!ImGui::BeginTable(propertyGroupName, 2, ImGuiTableFlags_SizingFixedFit))
			return false;
		// NOTE: consider exposing this to some sort of settings
		const float labelColumnWidth = 100.0f;
		ImGui::TableSetupColumn("label_column", 0, labelColumnWidth);
		ImGui::TableSetupColumn("value_column",
			ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoClip,
			ImGui::GetContentRegionAvail().x - labelColumnWidth);

		return true;
	}

	void UI::EndPropertyGroup()
	{
		ImGui::EndTable();
	}

	bool UI::BeginPopupContextWindow(const char* name, ImGuiPopupFlags popupFlags)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		if (!name)
			name = "window_context";
		ImGuiID id = window->GetID(name);
		int mouse_button = (popupFlags & ImGuiPopupFlags_MouseButtonMask_);
		if (ImGui::IsMouseReleased(mouse_button) && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
			if (!(popupFlags& ImGuiPopupFlags_NoOpenOverItems) || !ImGui::IsAnyItemHovered())
				ImGui::OpenPopupEx(id, popupFlags);

		ImGuiWindowFlags popupWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
										ImGuiWindowFlags_NoTitleBar |
										ImGuiWindowFlags_NoSavedSettings |
										ImGuiWindowFlags_NoMove;
		return ImGui::BeginPopupEx(id, popupWindowFlags);
	}

	bool UI::BeginPopupContextItem(const char* name, ImGuiPopupFlags popupFlags)
	{
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		if (window->SkipItems)
			return false;
		ImGuiID id = name ? window->GetID(name) : g.LastItemData.ID;    // If user hasn't passed an ID, we can use the LastItemID. Using LastItemID as a Popup ID won't conflict!
		IM_ASSERT(id != 0);                                             // You cannot pass a NULL str_id if the last item has no identifier (e.g. a Text() item)
		int mouse_button = (popupFlags & ImGuiPopupFlags_MouseButtonMask_);
		if (ImGui::IsMouseReleased(mouse_button) && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
			ImGui::OpenPopupEx(id, popupFlags);
		ImGuiWindowFlags popupWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
											ImGuiWindowFlags_NoTitleBar |
											ImGuiWindowFlags_NoSavedSettings |
											ImGuiWindowFlags_NoMove;

		return ImGui::BeginPopupEx(id, popupWindowFlags);
	}

	static constexpr float power = 0.1f;

	bool UI::PropertyColor(std::string_view label, glm::vec4& value)
	{
		ImGui::PushID(label.data());
		bool changed = false;

		ImGui::TableSetColumnIndex(0);
		ImGui::Text(label.data());

		ImGui::TableSetColumnIndex(1);
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { ImGui::GetContentRegionAvail().x, lineHeight };
		
		if (ImGui::ColorButton("##colbutton", ImVec4{ value.r, value.g, value.b, value.a }, 0, buttonSize))
			ImGui::OpenPopup("picker");

		if (ImGui::BeginPopup("picker"))
		{
			changed = ImGui::ColorPicker4("##colpicker", glm::value_ptr(value));
			ImGui::EndPopup();
		}
		ImGui::PopID();

		return changed;
	}

	bool UI::PropertyVec3(const std::string& label, glm::vec3& value)
	{
		ImGui::PushID(label.c_str());
		bool changed = false;

		ImGui::TableSetColumnIndex(0);
		ImGui::Text(label.c_str());

		ImGui::TableSetColumnIndex(1);
		const ImGuiIO io = ImGui::GetIO();

		constexpr float itemSpacingX = 1.0f;
		ScopedStyleVar itemSpacingStyleVar({
			{ ImGuiStyleVar_ItemSpacing, { itemSpacingX, 0.0f } }
			});

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 2.0f, lineHeight };
		const float inputItemWidth = (ImGui::GetContentRegionAvail().x - itemSpacingX) / 3.0f - buttonSize.x;
		ScopedStyleColor buttonStyleColor({
			{ ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f } },
			{ ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f } },
			{ ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f } }
			});

		auto drawControl = [&](const std::string& label, float& oValue)
		{
			ImGui::Button(label.c_str(), buttonSize);
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
				oValue += io.MouseDelta.x * power;
			}

			ImGui::SameLine();
			ImGui::SetNextItemWidth(inputItemWidth);
			changed |= UI::DragScalar<float>(("##DR" + label).c_str(), &oValue, power, "%.2f");
		};

		drawControl("X", value.x);
		ImGui::SameLine();

		drawControl("Y", value.y);
		ImGui::SameLine();

		drawControl("Z", value.z);
		ImGui::PopID();

		return changed;
	}

	bool UI::PropertyVec2(const std::string& label, glm::vec2& value)
	{
		ImGui::PushID(label.c_str());
		bool changed = false;

		ImGui::TableSetColumnIndex(0);
		ImGui::Text(label.c_str());

		ImGui::TableSetColumnIndex(1);

		const ImGuiIO io = ImGui::GetIO();

		constexpr float itemSpacingX = 1.0f;
		ScopedStyleVar itemSpacingStyleVar({
			{ ImGuiStyleVar_ItemSpacing, { itemSpacingX, 0.0f } }
		});

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 2.0f, lineHeight };
		const float inputItemWidth = (ImGui::GetContentRegionAvail().x - itemSpacingX) / 2.0f - buttonSize.x;

		ScopedStyleColor buttonStyleColor({
			{ ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f } },
			{ ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f } },
			{ ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f } }
		});

		auto drawControl = [&](const std::string& label, float& oValue)
		{
			ImGui::Button(label.c_str(), buttonSize);
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
				oValue += io.MouseDelta.x * power;
			}

			ImGui::SameLine();
			ImGui::SetNextItemWidth(inputItemWidth);
			changed |= UI::DragScalar<float>(("##DR" + label).c_str(), &oValue, power, "%.2f");
		};

		drawControl("X", value.x);
		ImGui::SameLine();

		drawControl("Y", value.y);
		ImGui::PopID();

		return changed;
	}

	bool UI::PropertyEntity(const std::string& label, UUID& value, const Shared<Scene>& scene, float columnWidth)
	{
		ImGui::PushID(label.c_str());
		bool modified = false;

		ImGui::TableSetColumnIndex(0);
		ImGui::Text(label.c_str());

		ImGui::TableSetColumnIndex(1);

		Entity entity = scene->FindEntityWithUUID(value);

		char buf[256];
		memset(buf, 0, sizeof(buf));
		strcpy_s(buf, sizeof(buf), !entity ? "None" : entity.GetName().c_str());

		ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_ReadOnly;
		ImGui::InputText("##EntityField", buf, sizeof(buf), inputTextFlags);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_UUID"))
			{
				TR_ASSERT(payload->DataSize == 16 * sizeof(uint8_t), "The Drag/Drop Payload data's size doesn't match the required size");
				UUID id = UUID::CreateFromRaw((uint8_t*)payload->Data);
				Entity receivedEntity = scene->FindEntityWithUUID(id);

				if (receivedEntity)
				{
					value = receivedEntity.GetID();
					modified = true;
				}
			}

			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();

		return modified;
	}

	bool UI::PropertyFloat(const std::string& label, float& value)
	{
		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::Text(label.c_str());

		ImGui::TableSetColumnIndex(1);

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		bool changed = UI::DragScalar<float>(("##DR" + label).c_str(), &value, power, "%.2f");
		ImGui::PopID();

		return changed;
	}

	bool UI::PropertyInt(const std::string& label, int& value)
	{
		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::Text(label.c_str());

		ImGui::TableSetColumnIndex(1);

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		bool changed = UI::DragScalar<int>(("##DR" + label).c_str(), &value, power, "%.2f");
		ImGui::PopID();

		return changed;
	}

	bool UI::PropertyBool(const std::string& label, bool& value)
	{
		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::Text(label.c_str());

		ImGui::TableSetColumnIndex(1);

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		bool changed = ImGui::Checkbox("##val", &value);
		ImGui::PopID();

		return changed;
	}

	// TODO: support wide strings
	bool UI::PropertyString(const std::string& label, std::string& value, ImGuiInputTextFlags flags, int maxBufSize, float columnWidth)
	{
		ImGui::PushID(label.c_str());
		bool changed = false;

		ImGui::TableSetColumnIndex(0);
		ImGui::Text(label.c_str());

		ImGui::TableSetColumnIndex(1);

		char* buf = new char[maxBufSize];
		memset(buf, 0, maxBufSize);
		strcpy_s(buf, maxBufSize, value.c_str());

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::InputText("##val", buf, maxBufSize, flags))
		{
			value = buf;
			changed = true;
		}

		delete[] buf;
		ImGui::PopID();

		return changed;
	}

	// TODO: support wide strings
	bool UI::PropertyChar(const std::string& label, char& value)
	{
		std::string strVal; strVal += value;
		bool changed = PropertyString(label, strVal, 0, 2);

		if (strVal.empty()) return false;
		value = strVal.at(0);

		return changed;
	}

	bool UI::Button(const std::string& label, const char* buttonLabel)
	{
		ImGui::PushID(label.c_str());
		ImGui::TableSetColumnIndex(0);
		ImGui::Text(label.c_str());

		ImGui::TableSetColumnIndex(1);

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		bool changed = ImGui::Button(buttonLabel);
		ImGui::PopID();

		return changed;
	}

	bool UI::TreeNodeBehavior(ImGuiID id, ImGuiTreeNodeFlags flags, const char* label, const char* label_end)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const bool display_frame = (flags & ImGuiTreeNodeFlags_Framed) != 0;
		const ImVec2 padding = (display_frame || (flags & ImGuiTreeNodeFlags_FramePadding)) ? style.FramePadding : ImVec2(style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, style.FramePadding.y));

		if (!label_end)
			label_end = ImGui::FindRenderedTextEnd(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, label_end, false);

		// We vertically grow up to current line height up the typical widget height.
		const float frame_height = ImMax(ImMin(window->DC.CurrLineSize.y, g.FontSize + style.FramePadding.y * 2), label_size.y + padding.y * 2);
		ImRect frame_bb;
		frame_bb.Min.x = (flags & ImGuiTreeNodeFlags_SpanFullWidth) ? window->WorkRect.Min.x : window->DC.CursorPos.x;
		frame_bb.Min.y = window->DC.CursorPos.y;
		frame_bb.Max.x = window->WorkRect.Max.x;
		frame_bb.Max.y = window->DC.CursorPos.y + frame_height;
		if (display_frame)
		{
			// Framed header expand a little outside the default padding, to the edge of InnerClipRect
			// (FIXME: May remove this at some point and make InnerClipRect align with WindowPadding.x instead of WindowPadding.x*0.5f)
			frame_bb.Min.x -= IM_FLOOR(window->WindowPadding.x * 0.5f - 1.0f);
			frame_bb.Max.x += IM_FLOOR(window->WindowPadding.x * 0.5f);
		}

		const float text_offset_x = g.FontSize + (display_frame ? padding.x * 3 : padding.x * 2);           // Collapser arrow width + Spacing
		const float text_offset_y = ImMax(padding.y, window->DC.CurrLineTextBaseOffset);                    // Latch before ItemSize changes it
		const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f);  // Include collapser
		ImVec2 text_pos(window->DC.CursorPos.x + text_offset_x, window->DC.CursorPos.y + text_offset_y);
		ImGui::ItemSize(ImVec2(text_width, frame_height), padding.y);

		// For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
		ImRect interact_bb = frame_bb;
		if (!display_frame && (flags & (ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth)) == 0)
			interact_bb.Max.x = frame_bb.Min.x + text_width + style.ItemSpacing.x * 2.0f;

		// Store a flag for the current depth to tell if we will allow closing this node when navigating one of its child.
		// For this purpose we essentially compare if g.NavIdIsAlive went from 0 to 1 between TreeNode() and TreePop().
		// This is currently only support 32 level deep and we are fine with (1 << Depth) overflowing into a zero.
		const bool is_leaf = (flags & ImGuiTreeNodeFlags_Leaf) != 0;
		bool is_open = ImGui::TreeNodeUpdateNextOpen(id, flags);
		if (is_open && !g.NavIdIsAlive && (flags & ImGuiTreeNodeFlags_NavLeftJumpsBackHere) && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			window->DC.TreeJumpToParentOnPopMask |= (1 << window->DC.TreeDepth);

		bool item_add = ImGui::ItemAdd(interact_bb, id);
		g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HasDisplayRect;
		g.LastItemData.DisplayRect = frame_bb;

		if (!item_add)
		{
			if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
				ImGui::TreePushOverrideID(id);
			IMGUI_TEST_ENGINE_ITEM_INFO(g.LastItemData.ID, label, g.LastItemData.StatusFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
			return is_open;
		}

		ImGuiButtonFlags button_flags = ImGuiTreeNodeFlags_None;
		if (flags & ImGuiTreeNodeFlags_AllowItemOverlap)
			button_flags |= ImGuiButtonFlags_AllowOverlap;
		
		button_flags |= ImGuiButtonFlags_PressedOnDragDropHold;

		// We allow clicking on the arrow section with keyboard modifiers held, in order to easily
		// allow browsing a tree while preserving selection with code implementing multi-selection patterns.
		// When clicking on the rest of the tree node we always disallow keyboard modifiers.
		const float arrow_hit_x1 = (text_pos.x - text_offset_x) - style.TouchExtraPadding.x;
		const float arrow_hit_x2 = (text_pos.x - text_offset_x) + (g.FontSize + padding.x * 2.0f) + style.TouchExtraPadding.x;
		const bool is_mouse_x_over_arrow = (g.IO.MousePos.x >= arrow_hit_x1 && g.IO.MousePos.x < arrow_hit_x2);
		if (window != g.HoveredWindow || !is_mouse_x_over_arrow)
			button_flags |= ImGuiButtonFlags_NoKeyModifiers;

		// Open behaviors can be altered with the _OpenOnArrow and _OnOnDoubleClick flags.
		// Some alteration have subtle effects (e.g. toggle on MouseUp vs MouseDown events) due to requirements for multi-selection and drag and drop support.
		// - Single-click on label = Toggle on MouseUp (default, when _OpenOnArrow=0)
		// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=0)
		// - Single-click on arrow = Toggle on MouseDown (when _OpenOnArrow=1)
		// - Double-click on label = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1)
		// - Double-click on arrow = Toggle on MouseDoubleClick (when _OpenOnDoubleClick=1 and _OpenOnArrow=0)
		// It is rather standard that arrow click react on Down rather than Up.
		// We set ImGuiButtonFlags_PressedOnClickRelease on OpenOnDoubleClick because we want the item to be active on the initial MouseDown in order for drag and drop to work.
		if (is_mouse_x_over_arrow)
			button_flags |= ImGuiButtonFlags_PressedOnClick;
		else if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
			button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick;
		else
			button_flags |= ImGuiButtonFlags_PressedOnClickRelease;

		bool selected = (flags & ImGuiTreeNodeFlags_Selected) != 0;
		const bool was_selected = selected;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(interact_bb, id, &hovered, &held, button_flags);
		bool toggled = false;
		if (!is_leaf)
		{
			if (pressed && g.DragDropHoldJustPressedId != id)
			{
				if ((flags & (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick)) == 0 || (g.NavActivateId == id))
					toggled = true;
				if (flags & ImGuiTreeNodeFlags_OpenOnArrow)
					toggled |= is_mouse_x_over_arrow && !g.NavDisableMouseHover; // Lightweight equivalent of IsMouseHoveringRect() since ButtonBehavior() already did the job
				if ((flags & ImGuiTreeNodeFlags_OpenOnDoubleClick) && g.IO.MouseClickedCount[0] == 2)
					toggled = true;
			}
			else if (pressed && g.DragDropHoldJustPressedId == id)
			{
				IM_ASSERT(button_flags & ImGuiButtonFlags_PressedOnDragDropHold);
				if (!is_open) // When using Drag and Drop "hold to open" we keep the node highlighted after opening, but never close it again.
					toggled = true;
			}

			if (g.NavId == id && g.NavMoveDir == ImGuiDir_Left && is_open)
			{
				toggled = true;
				ImGui::NavMoveRequestCancel();
			}
			if (g.NavId == id && g.NavMoveDir == ImGuiDir_Right && !is_open) // If there's something upcoming on the line we may want to give it the priority?
			{
				toggled = true;
				ImGui::NavMoveRequestCancel();
			}

			if (toggled)
			{
				is_open = !is_open;
				window->DC.StateStorage->SetInt(id, is_open);
				g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledOpen;
			}
		}
		if (flags & ImGuiTreeNodeFlags_AllowItemOverlap)
			ImGui::SetItemAllowOverlap();

		// In this branch, TreeNodeBehavior() cannot toggle the selection so this will never trigger.
		if (selected != was_selected) //-V547
			g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

		// Render
		const ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
		ImGuiNavHighlightFlags nav_highlight_flags = ImGuiNavHighlightFlags_TypeThin;
		if (display_frame)
		{
			// Framed type
			const ImU32 bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
			ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, true, style.FrameRounding);
			ImGui::RenderNavHighlight(frame_bb, id, nav_highlight_flags);
			if (flags & ImGuiTreeNodeFlags_Bullet)
				ImGui::RenderBullet(window->DrawList, ImVec2(text_pos.x - text_offset_x * 0.60f, text_pos.y + g.FontSize * 0.5f), text_col);
			else if (!is_leaf)
				ImGui::RenderArrow(window->DrawList, ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y), text_col, is_open ? ImGuiDir_Down : ImGuiDir_Right, 1.0f);
			else // Leaf without bullet, left-adjusted text
				text_pos.x -= text_offset_x;
			if (flags & ImGuiTreeNodeFlags_ClipLabelForTrailingButton)
				frame_bb.Max.x -= g.FontSize + style.FramePadding.x;

			if (g.LogEnabled)
				ImGui::LogSetNextTextDecoration("###", "###");
			ImGui::RenderTextClipped(text_pos, frame_bb.Max, label, label_end, &label_size);
		}
		else
		{
			// Unframed typed for tree nodes
			if (hovered || selected)
			{
				const ImU32 bg_col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
				ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, bg_col, false);
			}
			ImGui::RenderNavHighlight(frame_bb, id, nav_highlight_flags);
			if (flags & ImGuiTreeNodeFlags_Bullet)
				ImGui::RenderBullet(window->DrawList, ImVec2(text_pos.x - text_offset_x * 0.5f, text_pos.y + g.FontSize * 0.5f), text_col);
			else if (!is_leaf)
				ImGui::RenderArrow(window->DrawList, ImVec2(text_pos.x - text_offset_x + padding.x, text_pos.y + g.FontSize * 0.15f), text_col, is_open ? ImGuiDir_Down : ImGuiDir_Right, 0.70f);
			if (g.LogEnabled)
				ImGui::LogSetNextTextDecoration(">", NULL);
			ImGui::RenderText(text_pos, label, label_end, false);
		}

		if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
			ImGui::TreePushOverrideID(id);
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | (is_leaf ? 0 : ImGuiItemStatusFlags_Openable) | (is_open ? ImGuiItemStatusFlags_Opened : 0));
		return is_open;
	}

	bool UI::TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		return TreeNodeBehavior(window->GetID(label), flags, label, NULL);
	}

	template<typename T>
	static void DrawFieldValue(int32_t fieldHandle, const std::string& fieldName, const Shared<ScriptInstance>& scriptInstance,
		const std::function<bool(const std::string& fieldName, T& value)>& drawFunc)
	{
		T value = scriptInstance->GetFieldValue<T>(fieldHandle);
		std::string proccessedFieldName = ProccessFieldName(fieldName);
		if (drawFunc(proccessedFieldName, value))
			scriptInstance->SetFieldValue<T>(fieldHandle, value);
	}

#define DRAW_FIELD_PROPERTY(FieldType, Type, DrawFunc, ...)				\
	case ScriptType::FieldType:											\
	{																	\
		DrawFieldValue<Type>(fieldHandle, field.Name, scriptInstance,	\
		[&__VA_ARGS__](const std::string& fieldName, auto& value)		\
		{																\
			return DrawFunc(fieldName, value, __VA_ARGS__);				\
		});																\
	}																	\
	break

#define DRAW_FIELD_PROPERTY_SCALAR(FieldType, Type)						\
	case ScriptType::FieldType:											\
	{																	\
		DrawFieldValue<Type>(fieldHandle, field.Name, scriptInstance,	\
		[](const std::string& fieldName, auto& value)					\
		{																\
			return PropertyScalar(fieldName, value);					\
		});																\
	}																	\
	break

	void UI::PropertyScriptField(const Shared<Scene>& scene, int32_t fieldHandle, const Shared<ScriptInstance>& scriptInstance)
	{
		TR_ASSERT(scriptInstance, "Invalid script instance");
		
		/*const ManagedClass& typeClass = field->GetType().GetTypeClass();*/
		/*std::vector<ScriptField> enumFields;*/

		const ScriptField& field = scriptInstance->GetScriptField(fieldHandle);

		switch (field.Type)
		{
		DRAW_FIELD_PROPERTY_SCALAR(Int8, int8_t);
		DRAW_FIELD_PROPERTY_SCALAR(Int16, int16_t);
		DRAW_FIELD_PROPERTY_SCALAR(Int32, int32_t);
		DRAW_FIELD_PROPERTY_SCALAR(Int64, int64_t);
		DRAW_FIELD_PROPERTY_SCALAR(UInt8, uint8_t);
		DRAW_FIELD_PROPERTY_SCALAR(UInt16, uint16_t);
		DRAW_FIELD_PROPERTY_SCALAR(UInt32, uint32_t);
		DRAW_FIELD_PROPERTY_SCALAR(UInt64, uint64_t);
		DRAW_FIELD_PROPERTY_SCALAR(Float, float);
		DRAW_FIELD_PROPERTY_SCALAR(Double, double);
		DRAW_FIELD_PROPERTY(Bool, bool, PropertyBool);
		DRAW_FIELD_PROPERTY(Char, char, PropertyChar);
		DRAW_FIELD_PROPERTY(String, std::string, PropertyString);
		DRAW_FIELD_PROPERTY(Vector2, glm::vec2, PropertyVec2);
		DRAW_FIELD_PROPERTY(Vector3, glm::vec3, PropertyVec3);
		DRAW_FIELD_PROPERTY(Color, glm::vec4, PropertyColor);
		DRAW_FIELD_PROPERTY(Entity, UUID, PropertyEntity, scene);
		}
	}

#if 0
		const char** enumFieldNames = nullptr;

		if (field->GetType().IsEnum())
		{
			enumFields = typeClass->GetEnumFields();

			enumFieldNames = new const char* [enumFields.size()];

			for (size_t i = 0; i < enumFields.size(); i++)
				enumFieldNames[i] = enumFields[i].GetName();
		}

		switch (field->GetType().TypeEnum)
		{
		case ScriptType::Bool:
		{
			DrawFieldValue<bool>(field, handle,
				[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return PropertyBool(fieldName, value);
				});

			break;
		}
		case ScriptType::Char:
		{
			DrawFieldValue<wchar_t>(field, handle,
				[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					// TODO: support wide strings
					std::string strVal; strVal += (char)value;
			bool changed = PropertyString(fieldName, strVal, 0, 2);

			if (strVal.empty()) return false;
			value = strVal.at(0);

			return changed;
				});

			break;
		}
		case ScriptType::Int8:
		{
			DrawFieldValue<int8_t>(field, handle,
				[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return !fieldType.IsEnum() ? UI::PropertyScalar(fieldName, value) :
					UI::PropertyComboBox(fieldName, enumFieldNames, enumFields.size(), value);
				});

			break;
		}
		case ScriptType::Int16:
		{
			DrawFieldValue<int16_t>(field, handle,
				[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return !fieldType.IsEnum() ? UI::PropertyScalar(fieldName, value) :
					UI::PropertyComboBox(fieldName, enumFieldNames, enumFields.size(), value);
				});

			break;
		}
		case ScriptType::Int32:
		{
			DrawFieldValue<int32_t>(field, handle,
				[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return !fieldType.IsEnum() ? UI::PropertyScalar(fieldName, value) :
					UI::PropertyComboBox(fieldName, enumFieldNames, enumFields.size(), value);
				});

			break;
		}
		case ScriptType::Int64:
		{
			DrawFieldValue<int64_t>(field, handle,
				[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return !fieldType.IsEnum() ? UI::PropertyScalar(fieldName, value) :
					UI::PropertyComboBox(fieldName, enumFieldNames, enumFields.size(), value);
				});

			break;
		}
		case ScriptType::UInt8:
		{
			DrawFieldValue<uint8_t>(field, handle,
				[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return !fieldType.IsEnum() ? UI::PropertyScalar(fieldName, value) :
					UI::PropertyComboBox(fieldName, enumFieldNames, enumFields.size(), value);
				});

			break;
		}
		case ScriptType::UInt16:
		{
			DrawFieldValue<uint16_t>(field, handle,
				[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return !fieldType.IsEnum() ? UI::PropertyScalar(fieldName, value) :
					UI::PropertyComboBox(fieldName, enumFieldNames, enumFields.size(), value);
				});

			break;
		}
		case ScriptType::UInt32:
		{
			DrawFieldValue<uint16_t>(field, handle,
				[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return !fieldType.IsEnum() ? UI::PropertyScalar(fieldName, value) :
					UI::PropertyComboBox(fieldName, enumFieldNames, enumFields.size(), value);
				});

			break;
		}
		case ScriptType::UInt64:
		{
			DrawFieldValue<uint16_t>(field, handle,
				[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return !fieldType.IsEnum() ? UI::PropertyScalar(fieldName, value) :
					UI::PropertyComboBox(fieldName, enumFieldNames, enumFields.size(), value);
				});

			break;
		}
		case ScriptType::Float:
		{
			DrawFieldValue<float>(field, handle,
				[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return UI::PropertyScalar(fieldName, value);
				});

			break;
		}
		case ScriptType::Double:
		{
			DrawFieldValue<double>(field, handle,
				[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return UI::PropertyScalar(fieldName, value);
				});

			break;
		}
		case ScriptType::String:
		{
			DrawFieldValue<std::string>(field, handle,
				[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return PropertyString(fieldName, value);
				});

			break;
		}
		case ScriptType::Vector2:
		{
			DrawFieldValue<glm::vec2>(field, handle,
				[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return PropertyVec2(fieldName, value);
				});

			break;
		}
		case ScriptType::Vector3:
		{
			DrawFieldValue<glm::vec3>(field, handle,
				[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return PropertyVec3(fieldName, value);
				});

			break;
		}
		case ScriptType::Color:
		{
			DrawFieldValue<glm::vec4>(field, handle,
				[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return PropertyColor(fieldName, value);
				});

			break;
		}
		case ScriptType::Entity:
		{
			DrawFieldValue<UUID>(field, handle,
				[&](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					return PropertyEntity(fieldName, value, scene);
				});

			break;
		}
		// TODO: make better
		/*case ScriptType::LayerMask:
		{
			DrawFieldValue<uint16_t>(field, handle,
				[](const std::string& fieldName, auto& value, const ScriptType& fieldType)
				{
					std::vector<const char*> layerNames = PhysicsLayerManager::GetLayerNames();
			std::array<bool, PhysicsLayerManager::GetMaxLayerCount()> layerIndices = GetLayerIndicesFromMask(value);
			if (UI::PropertyComboBoxMulti(fieldName, layerNames.data(), layerNames.size(), layerIndices.data()))
			{
				value = GetLayerMaskFromIndices(layerIndices);
				return true;
			}

			return false;
				});

			break;
		}*/
		}

		if (enumFieldNames != nullptr)
			delete[] enumFieldNames;
#endif

	//}

#define DRAW_FIELD_ARRAY_VALUE_SCALAR(FieldType, Type)					\
	case ScriptType::FieldType:											\
	{																	\
		Type value = scriptInstance->GetFieldArrayValue<Type>(array, i);\
		if(UI::PropertyScalar<Type>(elementName, value))				\
		{																\
			scriptInstance->SetFieldArrayValue(array, value, i);		\
			hasChanged = false;											\
		}																\
	}																	\
	break

#define DRAW_FIELD_ARRAY_VALUE(FieldType, Type, DrawFunc, ...)			\
	case ScriptType::FieldType:											\
	{																	\
		Type value = scriptInstance->GetFieldArrayValue<Type>(array, i);\
		if(DrawFunc(elementName, value, __VA_ARGS__))					\
		{																\
			scriptInstance->SetFieldArrayValue(array, value, i);		\
			hasChanged = false;											\
		}																\
	}																	\
	break

	bool UI::PropertyScriptArrayField(const Shared<Scene>& scene, TerranEngine::ScriptArray& array, const TerranEngine::Shared<TerranEngine::ScriptInstance>& scriptInstance)
	{
		bool hasChanged = false;

		const ScriptField& field = scriptInstance->GetScriptField(array.FieldHandle);

		UI::PushID();
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth |
									ImGuiTreeNodeFlags_FramePadding |
									ImGuiTreeNodeFlags_AllowItemOverlap;

		ImGui::Unindent(20.0f);
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

		bool opened = ImGui::TreeNodeEx(field.Name.c_str(), flags);

		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 1.5f);

		ImGui::PushItemWidth(lineHeight * 1.5f);
		int32_t arrayLength = scriptInstance->GetFieldArrayLength(array);

		std::string name = fmt::format("##{0}{1}", field.Name.c_str(), "array_size");
		if (UI::DragScalar<int32_t>(name.c_str(), &arrayLength, 0.1f))
		{
			// TODO: when array isn't initialized in c# code
			/*if (array.Handle)
				array.Resize(arrayLength);
			else
				array = ScriptArray(field->GetType().GetTypeClass(), arrayLength);*/

			scriptInstance->ResizeFieldArray(array, arrayLength);

			hasChanged = true;
		}
		ImGui::PopItemWidth();

		ImGuiTreeNodeFlags elementsFlags = ImGuiTreeNodeFlags_SpanAvailWidth |
											ImGuiTreeNodeFlags_OpenOnArrow |
											ImGuiTreeNodeFlags_Leaf |
											ImGuiTreeNodeFlags_FramePadding;

		if (opened)
		{
			UI::BeginPropertyGroup("script_array_values");
			for (int32_t i = 0; i < arrayLength; i++)
			{
				ImGui::TableNextRow();
				std::string elementName = std::to_string(i);
				switch (field.Type)
				{
				DRAW_FIELD_ARRAY_VALUE(Bool, bool, PropertyBool);
				DRAW_FIELD_ARRAY_VALUE(Char, char, PropertyChar);
				DRAW_FIELD_ARRAY_VALUE_SCALAR(Int8, int8_t);
				DRAW_FIELD_ARRAY_VALUE_SCALAR(Int16, int16_t);
				DRAW_FIELD_ARRAY_VALUE_SCALAR(Int32, int32_t);
				DRAW_FIELD_ARRAY_VALUE_SCALAR(Int64, int64_t);
				DRAW_FIELD_ARRAY_VALUE_SCALAR(UInt8, uint8_t);
				DRAW_FIELD_ARRAY_VALUE_SCALAR(UInt16, uint16_t);
				DRAW_FIELD_ARRAY_VALUE_SCALAR(UInt32, uint32_t);
				DRAW_FIELD_ARRAY_VALUE_SCALAR(UInt64, uint64_t);
				DRAW_FIELD_ARRAY_VALUE_SCALAR(Float, float);
				DRAW_FIELD_ARRAY_VALUE_SCALAR(Double, double);
				DRAW_FIELD_ARRAY_VALUE(String, std::string, UI::PropertyString);
				DRAW_FIELD_ARRAY_VALUE(Vector2, glm::vec2, UI::PropertyVec2);
				DRAW_FIELD_ARRAY_VALUE(Vector3, glm::vec3, UI::PropertyVec3);
				DRAW_FIELD_ARRAY_VALUE(Color, glm::vec4, UI::PropertyColor);
				DRAW_FIELD_ARRAY_VALUE(Entity, UUID, PropertyEntity, scene);
				}
			}
			UI::EndPropertyGroup();

			ImGui::TreePop();
		}
		ImGui::Indent(20.0f);

		UI::PopID();

		return hasChanged;
	}
}
