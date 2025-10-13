#pragma once

#include "EditorPanel.h"

#include <string_view>

namespace TerranEditor {

class SettingsPanel : public EditorPanel {
public:
    SettingsPanel() = default;
    ~SettingsPanel() = default;

    virtual void OnRender() override;
    virtual void SetSceneContext(Terran::Core::Shared<TerranEngine::Scene> const& context) override { m_Scene = context; }
    virtual std::string_view GetName() override { return "Settings"; }

private:
    void DrawPhysicsSettings();

private:
    int m_LayerIndex = 0;
};

}
