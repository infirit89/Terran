#pragma once

#include "EditorPanel.h"

namespace TerranEditor
{
    class SettingsPanel : public EditorPanel
    {
    public:
        SettingsPanel() = default;
        ~SettingsPanel() = default;

        virtual void OnRender() override;
		virtual void SetSceneContext(const TerranEngine::Shared<TerranEngine::Scene>& context) override { m_Scene = context; }
        virtual const char* GetName() override { return "Settings"; }

    private:
        void DrawPhysicsSettings();

    private:
        int m_LayerIndex = 0;
    };
}
