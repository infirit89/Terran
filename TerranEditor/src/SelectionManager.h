#pragma once

#include "Scene/Entity.h"

#include <vector>

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
		static void Select(SelectionContext context, const TerranEngine::UUID& id);

		static void DeselectAll(SelectionContext context);
		static void Deselect(SelectionContext context, const TerranEngine::UUID& id);

		//static std::vector<TerranEngine::Entity> GetSelected();
		static const std::vector<TerranEngine::UUID>& GetSelected(SelectionContext context);
		static bool IsSelected(SelectionContext context, const TerranEngine::UUID& id);

	private:
		static std::unordered_map<SelectionContext, std::vector<TerranEngine::UUID>> s_Selections;
	};
}
