#pragma once

#include <glm/glm.hpp>

namespace Terran::Core::Math {

bool decompose_transform_matrix(glm::mat4 const& modelMatrix, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
glm::mat4 compose_transform_matrix(glm::vec3 const& translation, glm::vec3 const& rotation, glm::vec3 const& scale);

}

