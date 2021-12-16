#pragma once

#include "Scene/Scene.h"

namespace TerranEngine 
{
	class TransformSystem 
	{
	public:
		TransformSystem(Scene* scene);

		void Update();

	private:
		void UpdateChild(Entity entity);

	private:
		Scene* m_Scene;
	};
}