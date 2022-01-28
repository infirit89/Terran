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
		static void SetTransformPosition_Internal(uint32_t entityRuntimeID, glm::vec3& position);
		static void GetTransformPosition_Internal(uint32_t entityRuntimeID, glm::vec3& position);
		
		static void SetTransformRotation_Internal(uint32_t entityRuntimeID, glm::vec3& position);
		static void GetTransformRotation_Internal(uint32_t entityRuntimeID, glm::vec3& position);

		static void SetTransformScale_Internal(uint32_t entityRuntimeID, glm::vec3& position);
		static void GetTransformScale_Internal(uint32_t entityRuntimeID, glm::vec3& position);
	};
}
