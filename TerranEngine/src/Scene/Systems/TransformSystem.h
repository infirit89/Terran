#pragma once

#include "Scene/Scene.h"

namespace TerranEngine 
{
	class TransformSystem 
	{
	public:
		static void SetContext(Scene* scene) { s_Context = scene; }

		static void Update();

	private:
		static void UpdateEntityTransform(Entity entity);

		static Scene* s_Context;
	};
}