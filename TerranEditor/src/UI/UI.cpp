#include "UI.h"

#include "Terran.h"
#include "Core/Log.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace TerranEditor 
{
	using namespace TerranEngine;
	bool UI::DrawColor4Control(const std::string& label, glm::vec4& value, float columnWidth)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());
		
		{
			const ScopedVarTable::TableInfo tableInfo;
			ScopedVarTable colorControlTable(label, tableInfo);
			const ImVec2 buttonSize = ImVec2{ ImGui::CalcItemWidth(), 0.0f };

			if (ImGui::ColorButton("##colbutton", ImVec4{ value.r, value.g, value.b, value.a }, 0, buttonSize))
				ImGui::OpenPopup("picker");

			if (ImGui::BeginPopup("picker"))
			{
				changed = ImGui::ColorPicker4("##colpicker", glm::value_ptr(value));
				ImGui::EndPopup();
			}
		}
		
		ImGui::PopID();

		return changed;
	}

	bool UI::DrawVec2Control(const std::string& label, glm::vec2& value, float power, const char* format, float columnWidth)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());

		{
			const ImGuiIO io = ImGui::GetIO();
			
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

			ImVec2 cursorPos = ImGui::GetCursorPos();
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

	bool UI::DrawEntityControl(const std::string& label, Entity& value, float columnWidth)
	{
		ScopedVarTable::TableInfo tableInfo;
		ScopedVarTable textureTable("Sprite", tableInfo);

		char buf[256];
		memset(buf, 0, sizeof(buf));

		strcpy_s(buf, sizeof(buf), !value ? "None" : value.GetName().c_str());
						
		ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_DontRenderCursor | ImGuiInputTextFlags_DontChangeMouseCursorOnHover;
		ImGui::InputText("##TextureField", buf, sizeof(buf), inputTextFlags);

		if (ImGui::BeginDragDropTarget()) 
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_UUID")) 
			{
				TR_ASSERT(payload->DataSize == 16 * sizeof(uint8_t), "The Drag/Drop Payload data's size doesn't match the required size");
				std::array<uint8_t, 16> idArr = { 0 };
				memcpy(idArr._Elems, payload->Data, 16 * sizeof(uint8_t));
				UUID id(idArr);
				Entity receivedEntity = SceneManager::GetCurrentScene()->FindEntityWithUUID(id);

				if(receivedEntity)
				{
					value = receivedEntity;
					return true;
				}
			}

			ImGui::EndDragDropTarget();
		}

		return false;
	}

	bool UI::DrawVec3Control(const std::string& label, glm::vec3& value, float power, const char* format, float columnWidth)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());

		{
			const ImGuiIO io = ImGui::GetIO();
			
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

			ImVec2 cursorPos = ImGui::GetCursorPos();
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

	bool UI::DrawFloatControl(const std::string& label, float& value, float power, const char* format, float columnWidth)
	{
		bool changed = false;

		ImGui::PushID(label.c_str());
		
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

	bool UI::DrawIntControl(const std::string& label, int& value, float power, float columnWidth)
	{
		bool changed = false;

		ImGui::PushID(label.c_str());

		{
			ScopedVarTable::TableInfo tableInfo;
			ScopedVarTable intTable(label, tableInfo);

			if (ImGui::DragInt("##val", &value, power))
				changed = true;

			if (ImGui::IsItemHovered() || ImGui::IsItemActive())
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		}
		
		ImGui::PopID();

		return changed;
	}

	bool UI::DrawBoolControl(const std::string& label, bool& value, float columnWidth)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());

		{
			ScopedVarTable::TableInfo tableInfo;
			ScopedVarTable floatTable(label, tableInfo);
			changed = ImGui::Checkbox("##val", &value);
		}

		ImGui::PopID();

		return changed;
	}

	bool UI::DrawStringControl(const std::string& label, std::string& value, ImGuiInputTextFlags flags, int maxBufSize, float columnWidth)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());

		{
			const ScopedVarTable::TableInfo tableInfo;
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

	bool UI::DrawScalar(const std::string& label, ImGuiDataType type, void* value, float power, const char* format, float columnWidth)
	{
		bool changed = false;
		ImGui::PushID(label.c_str());
		
		{
			const ScopedVarTable::TableInfo tableInfo;
			ScopedVarTable floatTable(label, tableInfo);

			if (ImGui::DragScalar("##val", type, value, power, nullptr, nullptr, format))
				changed = true;

			if (ImGui::IsItemHovered() || ImGui::IsItemActive())
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

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

		ImGui::Columns(tableInfo.columnCount, tableID, tableInfo.border);
		ImGui::SetColumnWidth(0, tableInfo.firstColumnWidth);
		ImGui::Text(name.c_str());
		ImGui::NextColumn();

		if (tableInfo.itemCount > 1)
			ImGui::PushMultiItemsWidths(tableInfo.itemCount, ImGui::CalcItemWidth());
		else
			ImGui::PushItemWidth(ImGui::CalcItemWidth());

	}

	UI::ScopedVarTable::~ScopedVarTable()
	{
		if (m_TableInfo.itemCount == 1)
			ImGui::PopItemWidth();

		ImGui::Columns(1);

		//delete[] m_TableID;
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

	UI::ScopedStyleColor::~ScopedStyleColor() { ImGui::PopStyleColor((int)m_StyleColorListSize); }

	UI::ScopedStyleVar::ScopedStyleVar(std::initializer_list<StyleVar> styleVarList)
		: m_StyleVarListSize(styleVarList.size())
	{
		for (auto& styleVarIt : styleVarList)
		{
			ImVec2 styleVal = { styleVarIt.Val.x, styleVarIt.Val.y };
			ImGui::PushStyleVar(styleVarIt.StyleVarIdx, styleVal);
		}
	}

	UI::ScopedStyleVar::~ScopedStyleVar() { ImGui::PopStyleVar((int)m_StyleVarListSize); }
}

