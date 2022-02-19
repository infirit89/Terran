#pragma once

#include "Scene/Scene.h"

namespace TerranEngine 
{
	class TransformSystem 
	{
	public:
		static void Update(Scene* scene);

	private:
		static void UpdateEntityTransform(Entity entity);
	};
}