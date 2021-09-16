#pragma once

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace TerranEngine 
{
	class OrthographicCamera : public Camera
	{
	public: 
		OrthographicCamera(float width, float height);
		OrthographicCamera(float width, float height, float depth);
		OrthographicCamera(float left, float right, float bottom, float top);
		OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar);

		void SetViewport(float width, float height);
	private:
		void AdjustProjection(float left, float right, float bottom, float top, float zNear, float zFar);
		void AdjustProjection(float width, float height, float depth);
	};
}