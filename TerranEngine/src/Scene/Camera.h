#pragma once

#include <glm/mat4x4.hpp>

namespace TerranEngine 
{
	class Camera 
	{
	public:
		Camera(glm::mat4x4 projectionMatrix) 
			: m_ProjectionMatrix(projectionMatrix) {}

		const glm::mat4x4& GetProjection() { return m_ProjectionMatrix; }
	protected:
		glm::mat4x4 m_ProjectionMatrix;
	};
}