#pragma once

#include "Scene/Entity.h"

namespace TerranEditor 
{
	class SelectionManager 
	{
	public:
		static void Select(TerranEngine::Entity entity);
		static void Deselect();

		static TerranEngine::Entity GetSelected();

	private:
		static TerranEngine::Entity s_SelectedEntity;
	};
}
