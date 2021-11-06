#pragma once

#include <entt.hpp>

namespace TerranEngine 
{
	class Scene;

	class TransformSystem 
	{
	public:
		static void Update(Scene* scene);
	};
}