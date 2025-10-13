#pragma once

#include "LibCore/Base.h"

#include "Events/KeyboardEvent.h"
#include "LibCore/Event.h"

#include "Scene/Entity.h"
#include "Scene/Scene.h"

#include "EditorPanel.h"

#include <imgui.h>
#include <string_view>

namespace TerranEditor {

using namespace TerranEngine;
class SceneHierarchyPanel : public EditorPanel {
public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(Terran::Core::Shared<Scene> const& scene);

    ~SceneHierarchyPanel() = default;

    virtual void OnEvent(Terran::Core::Event& event) override;
    virtual void SetSceneContext(Terran::Core::Shared<TerranEngine::Scene> const& scene) override;
    virtual void OnRender() override;
    virtual std::string_view GetName() override { return "Hierarchy"; }

private:
    bool OnKeyPressed(KeyPressedEvent& e);
    void DrawEntityNode(Entity entity);
    void DrawScene();

private:
    ImGuiTextFilter m_Filter;
};

}
