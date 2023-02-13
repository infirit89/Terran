#pragma once

#include "Scene/Entity.h"

namespace TerranEditor 
{
	enum class SelectionContext 
	{
		Scene, ContentPanel
	};

	class SelectionManager 
	{
	public:
		static void Select(TerranEngine::Entity entity);
		static void Select(SelectionContext context, TerranEngine::UUID id);

		static void Deselect(SelectionContext context);

		static TerranEngine::Entity GetSelected();
		static TerranEngine::UUID GetSelectedID(SelectionContext context);

	private:
		static std::unordered_map<SelectionContext, TerranEngine::UUID> s_Selections;
	};
}
