#include "trpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace TerranEngine 
{
	OrthographicCamera::OrthographicCamera()
		: Camera(glm::mat4(1.0f))
	{
	}

	void OrthographicCamera::SetViewport(float width, float height) {

		m_AspectRatio = width / height;

		AdjustProjection();
	}

	void OrthographicCamera::AdjustProjection()
	{
		float width = (m_OrthographicSize * m_AspectRatio * 0.5f) * 2;
		float height = (m_OrthographicSize * 0.5f) * 2;
		m_ProjectionMatrix = glm::ortho(-width / 2, width / 2, -height / 2, height / 2, m_OrthographicNear, m_OrthographicFar);
	}
}

