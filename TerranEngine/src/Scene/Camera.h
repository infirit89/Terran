#pragma once

#include <glm/mat4x4.hpp>

namespace TerranEngine 
{
	class Camera 
	{
	public:
		Camera(glm::mat4x4 projectionMatrix) 
			: ProjectionMatrix(projectionMatrix) {}
	public:
		glm::mat4x4 ProjectionMatrix;
	};
}