#pragma once

#include <glm/glm.hpp>

namespace TerranEngine 
{
	struct Character 
	{
		float UV01x, UV01y;
		float UV02x, UV02y;

		glm::ivec2 Size;
		glm::ivec2 Bearing;

		uint32_t Advance;
	};
}