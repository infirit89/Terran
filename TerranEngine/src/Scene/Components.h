#pragma once

#include "Renderer/OrthographicCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <glm/gtx/quaternion.hpp>

namespace TerranEngine 
{
	struct TestComponent 
	{
		float x, y;
	};

	struct TransformComponent 
	{
		glm::vec3 Position, Scale, Rotation;

		TransformComponent(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f), const glm::vec3& rotation = glm::vec3(0.0f, 0.0f, 0.0f))
			: Position(position), Scale(scale), Rotation(rotation) {}

		glm::mat4& GetTransformMatrix()
		{
			glm::mat4 rot = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Position) *
				rot *
				glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct CameraComponent 
	{
		OrthographicCamera Camera;
		bool Primary;

		CameraComponent() = default;

	};

	struct SpriteRendererComponent 
	{
		glm::vec4 Color;

		SpriteRendererComponent()
			: Color({ 1.0f, 1.0f, 1.0f, 1.0f })
		{}
	};
}