#include "SelectionManager.h"

#include "Scene/SceneManager.h"

namespace TerranEditor
{
	using namespace TerranEngine;
	
	UUID SelectionManager::s_SelectedUUID;

	void SelectionManager::Select(Entity entity) { s_SelectedUUID = entity.GetID(); }
	void SelectionManager::Select(UUID id) 
	{
		if (!SceneManager::GetCurrentScene()->FindEntityWithUUID(id))
			return;

		s_SelectedUUID = id;
	}
	
	void SelectionManager::Deselect() { s_SelectedUUID = UUID({ 0 }); }
	
	Entity SelectionManager::GetSelected()
	{ return SceneManager::GetCurrentScene()->FindEntityWithUUID(s_SelectedUUID); }

	UUID SelectionManager::GetSelectedID() { return s_SelectedUUID; }
}
