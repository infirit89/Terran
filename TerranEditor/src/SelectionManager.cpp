#include "SelectionManager.h"

#include "Scene/SceneManager.h"

namespace TerranEditor
{
	using namespace TerranEngine;
	
	std::unordered_map<SelectionContext, TerranEngine::UUID> SelectionManager::s_Selections;

	void SelectionManager::Select(Entity entity) 
	{
		s_Selections[SelectionContext::Scene] = entity.GetID();
	}
	void SelectionManager::Select(SelectionContext context, UUID id) 
	{	
		s_Selections[context] = id;
	}
	
	void SelectionManager::Deselect(SelectionContext context) 
	{ 
		s_Selections[context] = UUID({0});
	}
	
	Entity SelectionManager::GetSelected()
	{ 
		return SceneManager::GetCurrentScene()->FindEntityWithUUID(s_Selections[SelectionContext::Scene]);
	}

	UUID SelectionManager::GetSelectedID(SelectionContext context)
	{
		return s_Selections[context]; 
	}
}
