#include "SelectionManager.h"

namespace TerranEditor
{
	using namespace TerranEngine;
	Entity SelectionManager::s_SelectedEntity;

	void SelectionManager::Select(Entity entity) { s_SelectedEntity = entity; }
	void SelectionManager::Deselect() { s_SelectedEntity = { }; }
	Entity SelectionManager::GetSelected() { return s_SelectedEntity; }
}
