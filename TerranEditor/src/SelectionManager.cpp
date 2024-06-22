#include "SelectionManager.h"

#include "Scene/SceneManager.h"

#include "Core/Log.h"

namespace TerranEditor
{
	using namespace TerranEngine;
	
	std::unordered_map<SelectionContext, std::vector<UUID>> SelectionManager::s_Selections;

	void SelectionManager::Select(Entity entity) 
	{
		auto& selected = s_Selections.at(SelectionContext::Scene);
		selected.push_back(entity.GetID());
	}
	void SelectionManager::Select(SelectionContext context, const UUID& id) 
	{
		s_Selections[context].push_back(id);
	}
	
	void SelectionManager::DeselectAll(SelectionContext context)
	{ 
		s_Selections[context].clear();
	}

	void SelectionManager::Deselect(SelectionContext context, const TerranEngine::UUID& id)
	{
		std::vector<UUID>& selected = s_Selections.at(context);
		auto it = std::find(selected.begin(), selected.end(), id);
		selected.erase(it);
	}
	
	//std::vector<Entity> SelectionManager::GetSelected()
	//{ 
	//	//return SceneManager::GetCurrentScene()->FindEntityWithUUID(s_Selections[SelectionContext::Scene]);
	//}

	const std::vector<UUID>& SelectionManager::GetSelected(SelectionContext context)
	{
		return s_Selections[context]; 
	}

	bool SelectionManager::IsSelected(SelectionContext context, const TerranEngine::UUID& id)
	{
		std::vector<UUID>& selected = s_Selections[context];
		auto it = std::find(selected.begin(), selected.end(), id);
		return it != selected.end();
	}
}
