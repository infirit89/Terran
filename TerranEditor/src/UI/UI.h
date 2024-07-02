#pragma once

#include "Core/Assert.h"

#include "Scripting/ScriptEngine.h"
#include "Scripting/ScriptInstance.h"

#include "Asset/AssetManager.h"
#include "Graphics/Texture.h"

#include "FontManager.h"

#include <imgui.h>

#include <glm/glm.hpp>

#include <string.h>
#include <string>
#include <unordered_map>
#include <typeindex>

namespace TerranEditor::UI
{
	// TODO: needs rework
	class ScopedVarTable 
	{
	public:
		struct TableInfo 
		{
			uint32_t ColumnCount = 2;
			float FirstColumnWidth = 100.0f;
			bool Border = false;
			uint32_t ItemCount = 1;
		};

		ScopedVarTable(const std::string& name, TableInfo tableInfo);
		~ScopedVarTable();

	private:
		TableInfo m_TableInfo;
	};

	class ScopedStyleColor 
	{
	public:
		struct StyleColor
		{
			int ColorVarIdx;
			ImVec4 Color;
		};

		ScopedStyleColor(std::initializer_list<StyleColor> styleColorList);
		~ScopedStyleColor();
	private:
		size_t m_StyleColorListSize = 0;
	};

	class ScopedStyleVar 
	{
	public:
		struct StyleVar 
		{
			StyleVar(int styleVarIndex, float fval) 
			{
				StyleVarIdx = styleVarIndex;
				Val.FVal = fval;
			}

			StyleVar(int styleVarIndex, ImVec2 vecVal) 
			{
				StyleVarIdx = styleVarIndex;
				Val.VecVal = vecVal;
			}

			~StyleVar() = default;

			int StyleVarIdx;

			union
			{
				ImVec2 VecVal;
				float FVal;
			} Val{};

			operator ImVec2() const { return Val.VecVal; }
			operator float() const { return Val.FVal; }
		};

		ScopedStyleVar(std::initializer_list<StyleVar> styleVarList);
		~ScopedStyleVar();

	private:
		size_t m_StyleVarListSize = 0;
	};

	class ScopedFont
	{
	public:
		ScopedFont(const std::string& fontName) 
		{
			FontManager::PushFont(fontName);
		}

		ScopedFont(ImFont* font)
		{
			ImGui::PushFont(font);
		}

		~ScopedFont()
		{
			FontManager::PopFont();
		}
	};
		
	void PushID();
	void PopID();
	int GenerateID();
	void PushFontSize(float fontSize);
	void PopFontSize();

	void ShiftCursor(float x, float y);
	void ShiftCursorX(float x);
	void ShiftCursorY(float y);
		
	void SetupUIStyle();
	void SetupUIStyle2();
	void SetupUIStyle3();
	void SetupUIStyle4();

	void Image(const TerranEngine::Shared<TerranEngine::Texture>& texture, const glm::vec2& size, const glm::vec2& uv0 = { 0, 1 }, const glm::vec2& uv1 = { 1, 0 }, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f});

	bool SearchInput(ImGuiTextFilter& filter, const std::string& hint, float width = 200.0f);

	void Tooltip(std::string_view text);

	bool BeginPropertyGroup(std::string_view propertyGroupName);
	void EndPropertyGroup();

	template<typename Func>
	void PropertyGroup(std::string_view propertyGroupName, Func&& func) 
	{
		if (!BeginPropertyGroup(propertyGroupName))
			return;

		func();

		EndPropertyGroup();
	}

	bool BeginPopupContextWindow(std::string_view name, ImGuiPopupFlags popupFlags = ImGuiPopupFlags_MouseButtonRight);
	bool BeginPopupContextItem(std::string_view name, ImGuiPopupFlags popupFlags = ImGuiPopupFlags_MouseButtonRight);

	inline std::unordered_map<std::type_index, ImGuiDataType> ImGuiDataTypeMap =
	{
		{ typeid(int8_t), ImGuiDataType_S8 },
		{ typeid(int16_t), ImGuiDataType_S16 },
		{ typeid(int32_t), ImGuiDataType_S32 },
		{ typeid(int64_t), ImGuiDataType_S64 },
		{ typeid(uint8_t), ImGuiDataType_U8 },
		{ typeid(uint16_t), ImGuiDataType_U16 },
		{ typeid(uint32_t), ImGuiDataType_U32 },
		{ typeid(uint64_t), ImGuiDataType_U64 },
		{ typeid(float), ImGuiDataType_Float },
		{ typeid(double), ImGuiDataType_Double }
	};

	bool PropertyColor(std::string_view label, glm::vec4& value);
	bool PropertyVec3(std::string_view label, glm::vec3& value);
	bool PropertyVec2(std::string_view label, glm::vec2& value);
	bool PropertyEntity(std::string_view label, TerranEngine::UUID& value, const TerranEngine::Shared<TerranEngine::Scene>& scene, float columnWidth = 100.0f);
	void PropertyScriptField(const TerranEngine::Shared<TerranEngine::Scene>& scene, int32_t fieldHandle, const TerranEngine::Shared<TerranEngine::ScriptInstance>& scriptInstance);
	bool PropertyScriptArrayField(const TerranEngine::Shared<TerranEngine::Scene>& scene, TerranEngine::ScriptArray& array, const TerranEngine::Shared<TerranEngine::ScriptInstance>& scriptInstance);
	bool PropertyFloat(std::string_view label, float& value);
	bool PropertyInt(std::string_view label, int& value);
	bool PropertyBool(std::string_view label, bool& value);
	bool PropertyString(std::string_view label, std::string& value, ImGuiInputTextFlags flags = 0, int maxBufSize = 256, float columnWidth = 100.0f);
	bool PropertyChar(std::string_view label, char& value);
	
	template<typename T>
	bool DragScalar(std::string_view label, T* value, float power = 0.1f, std::string_view format = nullptr, ImGuiSliderFlags flags = 0)
	{
		TR_ASSERT(ImGuiDataTypeMap.find(typeid(T)) != ImGuiDataTypeMap.end(), "Invalid data type");

		ImGuiDataType dataType = ImGuiDataTypeMap[typeid(T)];

		if constexpr (std::is_same_v<T, int8_t> ||
						std::is_same_v<T, int16_t> || 
						std::is_same_v<T, int32_t> || 
						std::is_same_v<T, int64_t>)
			format = "%d";
		else if constexpr (std::is_same_v<T, uint8_t> || 
							std::is_same_v<T, uint16_t> || 
							std::is_same_v<T, uint32_t> || 
							std::is_same_v<T, uint64_t>)
			format = "%u";

		bool modified = false;
				
		modified = ImGui::DragScalar(label.data(), dataType, value, power, nullptr, nullptr, format.data(), flags);
		return modified;
	}

	template<typename T>
	bool PropertyScalar(const std::string& label, T& value, float power = 0.1f)
	{
		ImGui::PushID(label.c_str());
		
		ImGui::TableSetColumnIndex(0);
		ImGui::Text(label.c_str());

		ImGui::TableSetColumnIndex(1);

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		bool changed = UI::DragScalar<T>(("##DR" + label).c_str(), &value, power, "%.2f");

		ImGui::PopID();

		return changed;
	}

	// this mostly same as imgui's but with some minor tweaks
	bool TreeNodeBehavior(ImGuiID id, ImGuiTreeNodeFlags flags, const char* label, const char* label_end);
	bool TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags);

	template<typename TEnum>
	bool PropertyDropdown(const std::string& label, std::string_view* stateNames, size_t stateCount, TEnum& selected)
	{
		ImGui::PushID(label.c_str());
		bool changed = false;
		std::string_view currentState = stateNames[(int32_t)selected];

		ImGui::TableSetColumnIndex(0);
		ImGui::Text(label.c_str());

		ImGui::TableSetColumnIndex(1);

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::BeginCombo("##ComboBox", currentState.data()))
		{
			for (size_t i = 0; i < stateCount; i++)
			{
				if (stateNames[i].size() == 0) continue;

				const bool isSelected = (int)selected == i;
				if (ImGui::Selectable(stateNames[i].data(), isSelected))
				{
					selected = (TEnum)i;
					changed = true;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::PopID();

		return changed;
	}

	template<typename TEnum, size_t TStateCount>
	bool PropertyDropdown(const std::string& label, std::string_view (&stateNames)[TStateCount], TEnum& selected) 
	{
		return PropertyDropdown(label, stateNames, TStateCount, selected);
	}

	template<typename TAsset>
	bool PropertyAssetField(const std::string& label, TerranEngine::AssetType type, TerranEngine::UUID& outHandle)
	{
		ImGui::PushID(label.c_str());
		bool changed = false;
			
		ImGui::TableSetColumnIndex(0);
		ImGui::Text(label.c_str());

		ImGui::TableSetColumnIndex(1);

		char buf[256];
		memset(buf, 0, sizeof(buf));
		TerranEngine::AssetInfo assetInfo = TerranEngine::AssetManager::GetAssetInfo(outHandle);
		std::string assetName = assetInfo.Path.stem().string();
		strcpy_s(buf, sizeof(buf), assetName == "" ? "None" : assetName.c_str());

		ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_ReadOnly;

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputText("##AssetField", buf, sizeof(buf), inputTextFlags);

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET"))
			{
				TerranEngine::UUID assetHandle = TerranEngine::UUID::CreateFromRaw((uint8_t*)payload->Data);
				TerranEngine::AssetInfo info = TerranEngine::AssetManager::GetAssetInfo(assetHandle);

				if (info.Type == type)
				{
					outHandle = assetHandle;
					changed = true;
				}
			}

			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();

		return changed;
	}

	bool PropertyDropdownMultipleSelect(const std::string& label, const char** stateNames, size_t stateCount, bool* selectedElements);
}
