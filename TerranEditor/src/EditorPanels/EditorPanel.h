#pragma once

#include "LibCore/Base.h"
#include "LibCore/Event.h"

#include "Scene/Scene.h"

#include <filesystem>
#include <string_view>

namespace TerranEditor {

class EditorPanel {
public:
    EditorPanel() = default;
    virtual ~EditorPanel() = default;

    virtual void SetSceneContext(Terran::Core::Shared<TerranEngine::Scene> const& scene) { m_Scene = scene; }
    virtual void OnRender() = 0;
    virtual void OnEvent(Terran::Core::Event& event) { }
    virtual void OnProjectChanged(std::filesystem::path const& projectPath) { }
    virtual std::string_view GetName() = 0;

    void SetOpen(bool open) { m_Open = open; }

protected:
    bool m_Open = true;
    Terran::Core::Shared<TerranEngine::Scene> m_Scene;
};

}
