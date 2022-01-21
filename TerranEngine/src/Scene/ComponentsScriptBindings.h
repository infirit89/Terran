#pragma once

#include <cstdint>

#include <glm/glm.hpp>

namespace TerranEngine 
{
	class ComponentsBindings 
	{
	public:
		static void Bind();

	private:
		static void SetTransformPosition_Internal(glm::vec3& position);
		static void GetTransformPosition_Internal(glm::vec3& position);
	};
}
