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
		
		bool DrawColor4Control(const std::string& label, glm::vec4& value, float columnWidth = 100.0f);
		bool DrawVec3Control(const std::string& label, glm::vec3& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);
		bool DrawFloatControl(const std::string& label, float& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);
		bool DrawIntControl(const std::string& label, int& value, float power = 0.1f, float columnWidth = 100.0f);
		bool DrawBoolControl(const std::string& label, bool& value, float columnWidth = 100.0f);
		bool DrawStringControl(const std::string& label, std::string& value, ImGuiInputTextFlags flags = 0, int maxBufSize = 256, float columnWidth = 100.0f);
		bool DrawScalar(const std::string& label, ImGuiDataType type, void* value, float power = 0.1f, const char* format = nullptr, float columnWidth = 100.0f);
		bool DrawVec2Control(const std::string& label, glm::vec2& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);
		bool DrawEntityControl(const std::string& label, TerranEngine::UUID& value, const TerranEngine::Shared<TerranEngine::Scene>& scene, float columnWidth = 100.0f);			
		

	}
}
