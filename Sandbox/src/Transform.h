#pragma once

#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace TerranEngine 
{
	struct Transform 
	{
		glm::vec3 Position, Scale;
		float Rotation;

		Transform(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const float& rotation = 0.0f, const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f))
			: Position(position), Rotation(rotation), Scale(scale) {}

		glm::mat4 GetTransformMatrix() 
		{
			return glm::translate(glm::mat4(1.0f), Position) *
				glm::rotate(glm::mat4(1.0f), Rotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
				glm::scale(glm::mat4(1.0f), Scale);
		}
	};
}
