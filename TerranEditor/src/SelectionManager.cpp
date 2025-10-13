#include "SelectionManager.h"

#include "Scene/SceneManager.h"

#include "LibCore/Log.h"

#include <algorithm>
#include <vector>

namespace TerranEditor {

using namespace TerranEngine;

std::unordered_map<SelectionContext, std::vector<Terran::Core::UUID>> SelectionManager::s_Selections;

void SelectionManager::Select(Entity entity)
{
    auto& selected = s_Selections.at(SelectionContext::Scene);
    selected.push_back(entity.GetID());
}
void SelectionManager::Select(SelectionContext context, Terran::Core::UUID const& id)
{
    s_Selections[context].push_back(id);
}

void SelectionManager::DeselectAll(SelectionContext context)
{
    s_Selections[context].clear();
}

void SelectionManager::Deselect(SelectionContext context, Terran::Core::UUID const& id)
{
    std::vector<Terran::Core::UUID>& selected = s_Selections.at(context);
    auto it = std::find(selected.begin(), selected.end(), id);
    selected.erase(it);
}

// std::vector<Entity> SelectionManager::GetSelected()
//{
//	//return SceneManager::GetCurrentScene()->FindEntityWithUUID(s_Selections[SelectionContext::Scene]);
// }

std::vector<Terran::Core::UUID> const& SelectionManager::GetSelected(SelectionContext context)
{
    return s_Selections[context];
}

bool SelectionManager::IsSelected(SelectionContext context, Terran::Core::UUID const& id)
{
    std::vector<Terran::Core::UUID>& selected = s_Selections[context];
    auto it = std::find(selected.begin(), selected.end(), id);
    return it != selected.end();
}

}
