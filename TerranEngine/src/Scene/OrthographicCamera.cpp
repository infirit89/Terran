#include "trpch.h"
#include "OrthographicCamera.h"

namespace TerranEngine 
{
	OrthographicCamera::OrthographicCamera(float width, float height)
		: Camera(glm::mat4x4(0))
	{
		AdjustProjection(width, height);
	}

	void OrthographicCamera::AdjustProjection(float left, float right, float bottom, float top)
	{
		ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	}

	void OrthographicCamera::AdjustProjection(float width, float height)
	{
		AdjustProjection(-width / 2, width / 2, -height / 2, height / 2);
	}
}

