#pragma once

#include <glm/glm.hpp>

namespace TerranEngine 
{
	struct Character 
	{
		int Index;

		glm::ivec2 Size;
		glm::ivec2 Bearing;

		uint32_t Advance;
	};
}