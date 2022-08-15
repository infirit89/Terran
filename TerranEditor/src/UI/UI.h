#pragma once

#include "Scripting/ScriptEngine.h"
#include <imgui.h>

#include <glm/glm.hpp>

#include <string>

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
				uint32_t columnCount = 2;
				float firstColumnWidth = 100.0f;
				bool border = false;
				uint32_t itemCount = 1;
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
				glm::vec4 Color;
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
				int StyleVarIdx;
				glm::vec2 Val;
			};

			ScopedStyleVar(std::initializer_list<StyleVar> styleVarList);
			~ScopedStyleVar();

		private:
			size_t m_StyleVarListSize = 0;
		};
		
		void SetupImGuiStyle();

		bool DrawColor4Control(const std::string& label, glm::vec4& value, float columnWidth = 100.0f);
		bool DrawVec3Control(const std::string& label, glm::vec3& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);
		bool DrawFloatControl(const std::string& label, float& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);
		bool DrawIntControl(const std::string& label, int& value, float power = 0.1f, float columnWidth = 100.0f);
		bool DrawBoolControl(const std::string& label, bool& value, float columnWidth = 100.0f);
		bool DrawStringControl(const std::string& label, std::string& value, ImGuiInputTextFlags flags = 0, int maxBufSize = 256, float columnWidth = 100.0f);

		template<typename T>
		bool DrawScalar(const std::string& label, T& value, float power = 0.1f, const char* format = nullptr, float columnWidth = 100.0f) 
		{
			ImGuiDataType dataType;

			if constexpr (std::is_same<T, int8_t>::value) dataType = ImGuiDataType_S8;
			if constexpr (std::is_same<T, int16_t>::value) dataType = ImGuiDataType_S16;
			if constexpr (std::is_same<T, int32_t>::value) dataType = ImGuiDataType_S32;
			if constexpr (std::is_same<T, int64_t>::value) dataType = ImGuiDataType_S64;
			if constexpr (std::is_same<T, uint8_t>::value) dataType = ImGuiDataType_U8;
			if constexpr (std::is_same<T, uint16_t>::value) dataType = ImGuiDataType_U16;
			if constexpr (std::is_same<T, uint32_t>::value) dataType = ImGuiDataType_U32;
			if constexpr (std::is_same<T, uint64_t>::value) dataType = ImGuiDataType_U64;
			if constexpr (std::is_same<T, float>::value) dataType = ImGuiDataType_Float;
			if constexpr (std::is_same<T, double>::value) dataType = ImGuiDataType_Double;

			bool changed = false;
			ImGui::PushID(label.c_str());

			{
				const ScopedVarTable::TableInfo tableInfo;
				ScopedVarTable floatTable(label, tableInfo);

				if (ImGui::DragScalar("##val", dataType, &value, power, nullptr, nullptr, format))
					changed = true;

				if (ImGui::IsItemHovered() || ImGui::IsItemActive())
					ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

			}

			ImGui::PopID();

			return changed;
		}

		bool DrawVec2Control(const std::string& label, glm::vec2& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);
		bool DrawEntityControl(const std::string& label, TerranEngine::UUID& value, const TerranEngine::Shared<TerranEngine::Scene>& scene, float columnWidth = 100.0f);			
		void DrawScriptField(const TerranEngine::Shared<TerranEngine::Scene>& scene, TerranEngine::ScriptField* field, const TerranEngine::GCHandle& handle);
		bool DrawScriptArrayField(const TerranEngine::Shared<TerranEngine::Scene>& scene, const std::string& fieldName, TerranEngine::ScriptArray& array);

		template<typename TEnum>
		bool DrawComboBox(const std::string& label, const char** stateNames, uint32_t stateCount, TEnum& selected) 
		{
			bool changed = false;
			const char* currentState = stateNames[(int32_t)selected];

			UI::ScopedVarTable::TableInfo tableInfo;
			UI::ScopedVarTable comboBoxTable(label, tableInfo);

            std::string comboHash = "##" + label;
			if (ImGui::BeginCombo(comboHash.c_str(), currentState))
			{
				for (int i = 0; i < stateCount; i++)
				{
					const bool is_selected = (stateNames[i] == currentState);
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

			return changed;
		}
	}
}

