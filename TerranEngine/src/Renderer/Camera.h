#pragma once

#include <glm/mat4x4.hpp>

namespace TerranEngine 
{
	class Camera 
	{
	public:
		Camera() 
			: m_ProjectionMatrix(glm::mat4(1.0f)), m_Width(0.0f), m_Height(0.0f), m_Depth(0.0f)
		{}
		Camera(glm::mat4 projectionMatrix) 
			: m_ProjectionMatrix(projectionMatrix), m_Width(0.0f), m_Height(0.0f), m_Depth(0.0f) {}

		const glm::mat4& GetProjection() const { return m_ProjectionMatrix; }
		const float GetWidth() { return m_Width; }
		const float GetHeight() { return m_Height; }
		const float GetDepth() { return m_Depth; }
	protected:
		glm::mat4 m_ProjectionMatrix;
		float m_Width, m_Height, m_Depth;
	};
}