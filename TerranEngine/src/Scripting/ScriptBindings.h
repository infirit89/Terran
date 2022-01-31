#pragma once
#include <mono/metadata/object.h>
#include <glm/glm.hpp>

namespace TerranEngine 
{
	class ScriptBindings 
	{
	public:
		static void Bind();

	private:
		// ---- Entity ----
		static bool HasComponent_Internal(uint32_t entityRuntimeID, MonoString* componentType);
		static void AddComponent_Internal(uint32_t entityRuntimeID, MonoString* componentType);
		static void RemoveComponent_Internal(uint32_t entityRuntimeID, MonoString* componentType);

		static uint32_t FindEntityWithName_Internal(MonoString* monoName);
		// ----------------

		// ---- Transform component ----
		static void SetTransformPosition_Internal(uint32_t entityRuntimeID, glm::vec3& inPosition);
		static void GetTransformPosition_Internal(uint32_t entityRuntimeID, glm::vec3& outPosition);

		static void SetTransformRotation_Internal(uint32_t entityRuntimeID, glm::vec3& inRotation);
		static void GetTransformRotation_Internal(uint32_t entityRuntimeID, glm::vec3& outRotation);

		static void SetTransformScale_Internal(uint32_t entityRuntimeID, glm::vec3& inScale);
		static void GetTransformScale_Internal(uint32_t entityRuntimeID, glm::vec3& outScale);
		// -----------------------------

		// ---- Tag component ----
		static void SetTagName_Internal(uint32_t entityRuntimeID, MonoString* inName);
		static MonoString* GetTagName_Internal(uint32_t entityRuntimeID);
		// -----------------------

		// ---- Utils ----
		static void Log_Internal(uint8_t logLevel, MonoString* monoMessage);
		// ---------------
	};
}