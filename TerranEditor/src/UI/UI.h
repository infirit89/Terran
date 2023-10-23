#pragma once

#include "Scripting/ScriptEngine.h"

#include "Assets/AssetManager.h"
#include "Graphics/Texture.h"

#include "FontManager.h"

#include <imgui.h>

#include <glm/glm.hpp>

#include <string.h>
#include <string>
#include <unordered_map>
#include <typeindex>

namespace TerranEditor
{
	// TODO: needs rework
	namespace UI 
	{
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
		
		void ShiftCursor(float x, float y);
		void ShiftCursorX(float x);
		void ShiftCursorY(float y);
		
		void SetupUIStyle();
		void SetupUIStyle2();
		void SetupUIStyle3();
		void SetupUIStyle4();

		void Image(const TerranEngine::Shared<TerranEngine::Texture>& texture, const glm::vec2& size, const glm::vec2& uv0 = { 0, 1 }, const glm::vec2& uv1 = { 1, 0 }, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f});

		bool SearchInput(ImGuiTextFilter& filter, const std::string& hint, float width = 200.0f);

		void Tooltip(const char* text);

		bool BeginPropertyGroup(const char* propertyGroupName);
		void EndPropertyGroup();

		bool BeginPopupContextWindow(const char* name, ImGuiPopupFlags popupFlags = ImGuiPopupFlags_MouseButtonRight);
		bool BeginPopupContextItem(const char* name, ImGuiPopupFlags popupFlags = ImGuiPopupFlags_MouseButtonRight);

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

		bool PropertyColor(const std::string& label, glm::vec4& value);
		bool PropertyVec3(const std::string& label, glm::vec3& value);
		bool PropertyVec2(const std::string& label, glm::vec2& value);
		bool PropertyEntity(const std::string& label, TerranEngine::UUID& value, const TerranEngine::Shared<TerranEngine::Scene>& scene, float columnWidth = 100.0f);
		void PropertyScriptField(const TerranEngine::Shared<TerranEngine::Scene>& scene, TerranEngine::ScriptField* field, const TerranEngine::GCHandle& handle);
		bool PropertyScriptArrayField(const TerranEngine::Shared<TerranEngine::Scene>& scene, const std::string& fieldName, TerranEngine::ScriptArray& array);
		bool PropertyFloat(const std::string& label, float& value);
		bool PropertyInt(const std::string& label, int& value);
		bool PropertyBool(const std::string& label, bool& value);
		bool PropertyString(const std::string& label, std::string& value, ImGuiInputTextFlags flags = 0, int maxBufSize = 256, float columnWidth = 100.0f);
		bool Button(const std::string& label, const char* buttonLabel);

		template<typename T>
		bool DragScalar(const char* label, T* value, float power = 0.1f, const char* format = nullptr, ImGuiSliderFlags flags = 0) 
		{
			TR_ASSERT(ImGuiDataTypeMap.find(typeid(T)) != ImGuiDataTypeMap.end(), "Invalid data type");

			ImGuiDataType dataType = ImGuiDataTypeMap[typeid(T)];

			if constexpr (std::is_same<T, int8_t>::value ||
						  std::is_same<T, int16_t>::value || 
						  std::is_same<T, int32_t>::value || 
						  std::is_same<T, int64_t>::value)
				format = "%d";
			else if constexpr (std::is_same<T, uint8_t>::value || 
							   std::is_same<T, uint16_t>::value || 
							   std::is_same<T, uint32_t>::value || 
							   std::is_same<T, uint64_t>::value)
				format = "%u";

			bool modified = false;
				
			modified = ImGui::DragScalar(label, dataType, value, power, nullptr, nullptr, format, flags);

			/*if (ImGui::IsItemHovered())
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);*/

			return modified;
		}

		template<typename T>
		bool PropertyScalar(const std::string& label, T& value) 
		{
			bool changed = false;
			ImGui::PushID(label.c_str());

			ImGui::TableSetColumnIndex(0);
			ImGui::Text(label.c_str());

			ImGui::TableSetColumnIndex(1);

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			changed = UI::DragScalar<T>(("##DR" + label).c_str(), &value, power, "%.2f");

			ImGui::PopID();

			return changed;
		}

		// the mostly same as imgui's but with some minor tweaks
		bool TreeNodeBehavior(ImGuiID id, ImGuiTreeNodeFlags flags, const char* label, const char* label_end);
		bool TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags);

		template<typename TEnum>
		bool PropertyDropdown(const std::string& label, const char** stateNames, size_t stateCount, TEnum& selected)
		{
			bool changed = false;
			const char* currentState = stateNames[(int32_t)selected];

			ImGui::PushID(label.c_str());
			
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(label.c_str());

			ImGui::TableSetColumnIndex(1);

			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::BeginCombo("##ComboBox", currentState))
			{
				for (size_t i = 0; i < stateCount; i++)
				{
                    if(strlen(stateNames[i]) == 0) continue;

					const bool is_selected = (int)selected == i;
					if (ImGui::Selectable(stateNames[i], is_selected))
					{
						selected = (TEnum)i;
						changed = true;
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::PopID();

			return changed;
		}

		template<typename TAsset>
		bool PropertyAssetField(const std::string& label, TerranEngine::AssetType type, TerranEngine::UUID& outHandle)
		{
			bool result = false;
			ImGui::PushID(label.c_str());
			
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
						result = true;
					}
				}

				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();

			return result;
		}

		bool PropertyDropdownMultipleSelect(const std::string& label, const char** stateNames, size_t stateCount, bool* selectedElements);
	}
}

