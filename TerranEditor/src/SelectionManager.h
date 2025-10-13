#pragma once

#include "Scene/Entity.h"

#include <vector>

namespace TerranEditor {

enum class SelectionContext {
    Scene,
    ContentPanel
};

class SelectionManager {
public:
    static void Select(TerranEngine::Entity entity);
    static void Select(SelectionContext context, Terran::Core::UUID const& id);

    static void DeselectAll(SelectionContext context);
    static void Deselect(SelectionContext context, Terran::Core::UUID const& id);

    // static std::vector<TerranEngine::Entity> GetSelected();
    static std::vector<Terran::Core::UUID> const& GetSelected(SelectionContext context);
    static bool IsSelected(SelectionContext context, Terran::Core::UUID const& id);

private:
    static std::unordered_map<SelectionContext, std::vector<Terran::Core::UUID>> s_Selections;
};

}
