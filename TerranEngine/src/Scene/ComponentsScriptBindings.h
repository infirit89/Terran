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

		// ---- Transform component ----
		static void SetTransformPosition_Internal(uint32_t entityRuntimeID, glm::vec3& inPosition);
		static void GetTransformPosition_Internal(uint32_t entityRuntimeID, glm::vec3& outPosition);
		
		static void SetTransformRotation_Internal(uint32_t entityRuntimeID, glm::vec3& inRotation);
		static void GetTransformRotation_Internal(uint32_t entityRuntimeID, glm::vec3& outRotation);

		static void SetTransformScale_Internal(uint32_t entityRuntimeID, glm::vec3& inScale);
		static void GetTransformScale_Internal(uint32_t entityRuntimeID, glm::vec3& outScale);
		// -----------------------------
	};
}
