#pragma once

#include <glm/glm.hpp>

namespace TerranEngine 
{
	bool Decompose(const glm::mat4& modelMatrix, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
}