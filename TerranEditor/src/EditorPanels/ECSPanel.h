#pragma once

#include "EditorPanel.h"

#include "LibCore/Base.h"
#include "Scene/Scene.h"

namespace TerranEditor {

using namespace TerranEngine;

class ECSPanel : public EditorPanel {
public:
    ECSPanel() = default;
    ~ECSPanel() = default;

    virtual void OnRender() override;

    virtual void SetSceneContext(Terran::Core::Shared<Scene> const& context) override { m_Scene = context; }
    virtual std::string_view GetName() override { return "ECS Debug"; }
};

}
