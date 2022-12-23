#pragma once

#include "EditorPanel.h"

namespace TerranEditor
{
    class SettingsPanel : public EditorPanel
    {
    public:
        SettingsPanel() = default;
        ~SettingsPanel() = default;

        virtual void ImGuiRender() override;
		virtual void SetSceneContext(const TerranEngine::Shared<TerranEngine::Scene>& context) override { m_Scene = context; }

    private:
        void DrawPhysicsSettings();

    private:
        int m_LayerIndex;
    };
}
