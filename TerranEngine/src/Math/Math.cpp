#include "trpch.h"
#include "Math.h"

#include <glm/gtx/matrix_decompose.hpp>

bool TerranEngine::Decompose(const glm::mat4& m, glm::vec3& pos, glm::vec3& eulerAngles, glm::vec3& scale)
{
	glm::mat4 LocalMatrix(m);

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
		// Clear the perspective partition
		LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<float>(0);
		LocalMatrix[3][3] = static_cast<float>(1);
	}
	
	// Next take care of translation (easy).
	pos = glm::vec3(LocalMatrix[3]);
	LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);

	glm::vec3 Row[3], Pdum3;

	// Now get scale and shear.
	for (glm::length_t i = 0; i < 3; ++i)
		for (glm::length_t j = 0; j < 3; ++j)
			Row[i][j] = LocalMatrix[i][j];

	// Compute X scale factor and normalize first row.
	scale.x = glm::length(Row[0]);// v3Length(Row[0]);

	Row[0] = glm::detail::scale(Row[0], static_cast<float>(1));

	// Compute XY shear factor and make 2nd row orthogonal to 1st.

	// Now, compute Y scale and normalize 2nd row.
	scale.y = glm::length(Row[1]);
	Row[1] = glm::detail::scale(Row[1], static_cast<float>(1));

	// Next, get Z scale and normalize 3rd row.
	scale.z = glm::length(Row[2]);
	Row[2] = glm::detail::scale(Row[2], static_cast<float>(1));
	
	// At this point, the matrix (in rows[]) is orthonormal.
	// Check for a coordinate system flip.  If the determinant
	// is -1, then negate the matrix and the scaling factors.
	Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
	if (dot(Row[0], Pdum3) < 0)
	{
		for (glm::length_t i = 0; i < 3; i++)
		{
			scale[i] *= static_cast<float>(-1);
			Row[i] *= static_cast<float>(-1);
		}
	}

	// Now, get the rotations out, as described in the gem.

	 eulerAngles.y = asin(-Row[0][2]);
	 if (cos(eulerAngles.y) != 0) {
	     eulerAngles.x = atan2(Row[1][2], Row[2][2]);
	     eulerAngles.z = atan2(Row[0][1], Row[0][0]);
	 } else {
		 eulerAngles.x = atan2(-Row[2][0], Row[1][1]);
		 eulerAngles.z = 0;
	 }

#if 0            // Make this function work with quaternions
	int i, j, k = 0;
	T root, trace = Row[0].x + Row[1].y + Row[2].z;
	if (trace > static_cast<T>(0))
	{
		root = sqrt(trace + static_cast<T>(1.0));
		Orientation.w = static_cast<T>(0.5) * root;
		root = static_cast<T>(0.5) / root;
		Orientation.x = root * (Row[1].z - Row[2].y);
		Orientation.y = root * (Row[2].x - Row[0].z);
		Orientation.z = root * (Row[0].y - Row[1].x);
	} // End if > 0
	else
	{
		static int Next[3] = { 1, 2, 0 };
		i = 0;
		if (Row[1].y > Row[0].x) i = 1;
		if (Row[2].z > Row[i][i]) i = 2;
		j = Next[i];
		k = Next[j];

		root = sqrt(Row[i][i] - Row[j][j] - Row[k][k] + static_cast<T>(1.0));

		Orientation[i] = static_cast<T>(0.5) * root;
		root = static_cast<T>(0.5) / root;
		Orientation[j] = root * (Row[i][j] + Row[j][i]);
		Orientation[k] = root * (Row[i][k] + Row[k][i]);
		Orientation.w = root * (Row[j][k] - Row[k][j]);
	} // End if <= 0

#endif

    return true;
}
