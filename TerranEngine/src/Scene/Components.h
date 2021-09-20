#pragma once

#include "Renderer/OrthographicCamera.h"
#include "Renderer/Texture.h"

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
		glm::vec3 Position, Scale;
		float Rotation;
		TransformComponent(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f), const float& rotation = 0.0f)
			: Position(position), Scale(scale), Rotation(rotation) {}

		glm::mat4 GetTransformMatrix()
		{
			//glm::mat4 rot = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Position) *
				glm::rotate(glm::mat4(1.0f), Rotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
				glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct CameraComponent 
	{
		OrthographicCamera Camera;
		bool Primary = true;

		CameraComponent() = default;

	};

	struct SpriteRendererComponent 
	{
		glm::vec4 Color;
		Shared<Texture> Texture;

		SpriteRendererComponent()
			: Color({ 1.0f, 1.0f, 1.0f, 1.0f })
		{}
	};
}