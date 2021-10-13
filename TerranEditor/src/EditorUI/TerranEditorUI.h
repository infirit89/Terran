#pragma once

#include <glm/glm.hpp>

namespace TerranEngine 
{
	class TerranEditorUI 
	{
	public:
		static void DrawColor4Control(const char* label, glm::vec4& value, float columnWidth = 100.0f);

		static bool DrawVec3Control(const char* label, glm::vec3& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);

		static bool DrawFloatControl(const char* label, float& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);
	};
}