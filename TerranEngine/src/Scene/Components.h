#pragma once

#include "Renderer/OrthographicCamera.h"

namespace TerranEngine 
{
	struct TestComponent 
	{
		float x, y;
	};

	struct CameraComponent 
	{
		OrthographicCamera Camera;

		CameraComponent() = default;

	};
}