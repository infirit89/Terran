#pragma once

#include <imgui.h>

#include <glm/glm.hpp>

#include <string>

namespace TerranEditor 
{
	namespace UI 
	{
		void DrawColor4Control(const char* label, glm::vec4& value, float columnWidth = 100.0f);

		bool DrawVec3Control(const char* label, glm::vec3& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);

		bool DrawFloatControl(const char* label, float& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);

		bool DrawIntControl(const char* label, int& value, float power = 0.1f, float columnWidth = 100.0f);

		bool DrawBoolControl(const char* label, bool& value, float columnWidth = 100.0f);

		bool DrawStringControl(const char* label, std::string& value, ImGuiInputTextFlags flags = 0, int maxBufSize = 256, float columnWidth = 100.0f);

		bool DrawScalar(const char* label, ImGuiDataType type, void* value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);
	};
}
