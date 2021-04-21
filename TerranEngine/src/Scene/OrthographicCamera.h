#pragma once

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace TerranEngine 
{
	class OrthographicCamera : public Camera
	{
	public: 
		OrthographicCamera(float width, float height);

		void SetViewport(float width, float height);
	private:
		void AdjustProjection(float left, float right, float bottom, float top);
		void AdjustProjection(float width, float height);
	};
}