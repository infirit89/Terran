#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "Scene/Camera.h"

namespace TerranEngine 
{
	class BatchRenderer 
	{
	public:
		static void Init(uint32_t batchSize);
		static void Close();

		static void BeginScene(const Camera& camera, glm::mat4& cameraTransform);
		static void Draw(glm::mat4& transform, glm::vec4 color);
		static void EndScene();

	private:
		static void Submit();
	};
}