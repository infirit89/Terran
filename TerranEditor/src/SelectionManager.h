#pragma once

#include "Scene/Entity.h"

namespace TerranEditor 
{
	class SelectionManager 
	{
	public:
		static void Select(TerranEngine::Entity entity);
		static void Select(TerranEngine::UUID id);

		static void Deselect();

		static TerranEngine::Entity GetSelected();
		static TerranEngine::UUID GetSelectedID();

	private:
		static TerranEngine::UUID s_SelectedUUID;
	};
}
