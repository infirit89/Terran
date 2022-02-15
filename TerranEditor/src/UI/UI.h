#pragma once

#include <glm/glm.hpp>

namespace TerranEditor 
{
	class UI 
	{
	public:
		static void DrawColor4Control(const char* label, glm::vec4& value, float columnWidth = 100.0f);

		static bool DrawVec3Control(const char* label, glm::vec3& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);

		static bool DrawFloatControl(const char* label, float& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);

		static bool DrawIntControl(const char* label, int& value, float power = 0.1f, float columnWidth = 100.0f);

		static bool DrawBoolControl(const char* label, bool& value, float columnWidth = 100.0f);
	};
}
