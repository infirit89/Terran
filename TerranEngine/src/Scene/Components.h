#pragma once

#include "Renderer/OrthographicCamera.h"
#include "Renderer/Texture.h"
#include "Core/Base.h"
#include "Core/UUID.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace TerranEngine 
{
	struct TestComponent 
	{
		float x, y;
	};

	struct TagComponent 
	{
		std::string Name;
		UUID ID;

		TagComponent() = default;

		TagComponent(const std::string& name, const UUID& id)
			: Name(name), ID(id) {}

		TagComponent(const std::string& name)
			: Name(name) {}
	};

	struct TransformComponent 
	{
		glm::vec3 Position, Scale;
		glm::vec3 Rotation;

		TransformComponent(const glm::vec3& position = { 0.0f, 0.0f, 0.0f }, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f }, const glm::vec3& rotation = { 0.0f, 0.0f, 0.0f })
			: Position(position), Scale(scale), Rotation(rotation) {}

		glm::mat4 GetTransformMatrix()
		{
			return glm::translate(glm::mat4(1.0f), Position) *
				glm::toMat4(glm::quat(Rotation)) *
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
		Shared<Texture> Texture = nullptr;

		SpriteRendererComponent()
			: Color({ 1.0f, 1.0f, 1.0f, 1.0f })
		{}
	};

	struct ScriptableComponent 
	{

	};
}