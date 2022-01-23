#pragma once

#include <mono/metadata/object.h>

namespace TerranEngine 
{
	class EntityBindings 
	{
	public:
		static void Bind();

	private:
		static bool HasComponent_Internal(uint32_t entityRuntimeID, MonoString* componentType);

		static void AddComponent_Internal(uint32_t entityRuntimeID, MonoString* componentType);

		static void RemoveComponent_Internal(uint32_t entityRuntimeID, MonoString* componentType);
	};
}