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
	protected:
		glm::mat4 m_ProjectionMatrix;
	};
}