#pragma once

#include <glm/glm.hpp>

namespace TerranEngine 
{
	class TerranEditorUI 
	{
	public:
		static void DrawVec3Control(const char* label, glm::vec3& value, float power = 0.1f, const char* format = "%.2f", float columnWidth = 100.0f);
	};
}