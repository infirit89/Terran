#include "trpch.h"
#include "Math.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace TerranEngine 
{
	bool Math::Decompose(const glm::mat4& modelMatrix, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
	{
		glm::mat4 LocalMatrix(modelMatrix);

		// Normalize the matrix.
		if (glm::epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), glm::epsilon<float>()))
			return false;

		for (glm::length_t i = 0; i < 4; ++i)
			for (glm::length_t j = 0; j < 4; ++j)
				LocalMatrix[i][j] /= LocalMatrix[3][3];

		if (
			glm::epsilonNotEqual(LocalMatrix[0][3], static_cast<float>(0), glm::epsilon<float>()) ||
			glm::epsilonNotEqual(LocalMatrix[1][3], static_cast<float>(0), glm::epsilon<float>()) ||
			glm::epsilonNotEqual(LocalMatrix[2][3], static_cast<float>(0), glm::epsilon<float>()))
		{
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<float>(0);
			LocalMatrix[3][3] = static_cast<float>(1);
		}

		translation = glm::vec3(LocalMatrix[3]);
		LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);

		glm::vec3 Row[3], Pdum3;

		for (glm::length_t i = 0; i < 3; ++i)
			for (glm::length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// get X scale factor and normalize first row.
		scale.x = glm::length(Row[0]);

		Row[0] = glm::detail::scale(Row[0], static_cast<float>(1));

		// get Y scale and normalize 2nd row.
		scale.y = glm::length(Row[1]);
		Row[1] = glm::detail::scale(Row[1], static_cast<float>(1));

		// get Z scale and normalize 3rd row.
		scale.z = glm::length(Row[2]);
		Row[2] = glm::detail::scale(Row[2], static_cast<float>(1));

		Pdum3 = cross(Row[1], Row[2]);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (glm::length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<float>(-1);
				Row[i] *= static_cast<float>(-1);
			}
		}

		// get the rotations out.
		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0) {
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else {
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}

		return true;
	}
}
