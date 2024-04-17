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
		Camera(glm::mat4 projectionMatrix) 
			: m_ProjectionMatrix(projectionMatrix) {}

		inline const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }

		glm::vec3 ScreenToWorld(const glm::mat4& viewMatrix, glm::vec3 screenPoint) 
		{
			return glm::inverse(m_ProjectionMatrix * viewMatrix) * glm::vec4(glm::normalize(screenPoint), 1.0f);
		}

		glm::vec3 WorldToScreen(const glm::mat4& viewMatrix, glm::vec3 worldPoint) {}
	protected:
		glm::mat4 m_ProjectionMatrix;
	};
}
