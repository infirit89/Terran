#pragma once

#include <glm/glm.hpp>

namespace TerranEngine 
{
	class Camera
	{
	public:
		Camera() 
			: m_ProjectionMatrix(glm::mat4(1.0f))
		{}
		Camera(const glm::mat4& projectionMatrix) 
			: m_ProjectionMatrix(projectionMatrix) {}

		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }

		glm::vec3 ScreenToWorld(const glm::mat4& viewMatrix, glm::vec3 screenPoint);

		glm::vec3 WorldToScreen(const glm::mat4& viewMatrix, glm::vec3 worldPoint) 
		{
			return {0.0f, 0.0f, 0.0f};
		}
	protected:
		glm::mat4 m_ProjectionMatrix;
	};
}
