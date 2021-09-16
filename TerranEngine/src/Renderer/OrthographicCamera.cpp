#include "trpch.h"
#include "OrthographicCamera.h"

namespace TerranEngine 
{
	OrthographicCamera::OrthographicCamera(float width, float height)
		: Camera(glm::mat4(0))
	{
		AdjustProjection(width, height, 20.0f);
	}

	OrthographicCamera::OrthographicCamera(float width, float height, float depth)
		: Camera(glm::mat4(0))
	{
		AdjustProjection(width, height, depth);
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: Camera(glm::mat4(0))
	{
		AdjustProjection(left, right, bottom, top, -10.0f, 10.0f);
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar)
		: Camera(glm::mat4(0))
	{
		AdjustProjection(left, right, bottom, top, zNear, zFar);
	}

	void OrthographicCamera::SetViewport(float width, float height) {

		AdjustProjection(width, height, m_Depth);
	}

	void OrthographicCamera::AdjustProjection(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		m_Width = right * 2;
		m_Height = top * 2;
		m_Depth = zFar * 2;

		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
	}

	void OrthographicCamera::AdjustProjection(float width, float height, float depth)
	{
		m_Width = width;
		m_Height = height;
		m_Depth = depth;
		AdjustProjection(-width / 2, width / 2, -height / 2, height / 2, -depth / 2, depth / 2);
	}
}

