#pragma once

#include <imgui.h>

#include <glm/glm.hpp>

#include <string>

namespace TerranEditor 
{
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

			ScopedVarTable(const char* name, TableInfo tableInfo);
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

		void DrawColor4Control(const char* label, glm::vec4& value, float columnWidth = 100.0f);

		bool DrawVec3Control(const char* label, glm::vec3& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);

		bool DrawFloatControl(const char* label, float& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);

		bool DrawIntControl(const char* label, int& value, float power = 0.1f, float columnWidth = 100.0f);

		bool DrawBoolControl(const char* label, bool& value, float columnWidth = 100.0f);

		bool DrawStringControl(const char* label, std::string& value, ImGuiInputTextFlags flags = 0, int maxBufSize = 256, float columnWidth = 100.0f);

		bool DrawScalar(const char* label, ImGuiDataType type, void* value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);

		bool DrawVec2Control(const char* label, glm::vec2& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);

	};
}
