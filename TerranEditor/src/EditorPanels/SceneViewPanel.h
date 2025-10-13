#pragma once

#include "EditorCamera.h"
#include "EditorPanel.h"

#include "LibCore/Base.h"

#include "Events/KeyboardEvent.h"

#include "Scene/Entity.h"
#include "Scene/Systems/SceneRenderer.h"

namespace TerranEditor {

using namespace TerranEngine;

enum class GizmoType {
    None = 0,
    Translaste,
    Rotate,
    Scale
};

class SceneViewPanel : public EditorPanel {
    // void OpenScene(const AssetInfo& sceneAssetInfo, glm::vec2 viewportSize);
    using OpenSceneFN = std::function<void(AssetInfo const&, glm::vec2 const&)>;

public:
    SceneViewPanel() = default;
    ~SceneViewPanel() = default;

    virtual void OnRender() override;

    void SetSceneRenderer(Terran::Core::Shared<SceneRenderer> const& sceneRenderer) { m_SceneRenderer = sceneRenderer; }

    // bool IsVisible() { return m_Visible; }

    virtual void OnEvent(Terran::Core::Event& event) override;

    void SetOpenSceneCallback(OpenSceneFN const& openSceneCallback) { m_OpenSceneCallback = openSceneCallback; }
    void SetViewportSizeChangedCallback(std::function<void(glm::vec2)> callback) { m_ViewportSizeChangedCallback = callback; }

    virtual void SetSceneContext(Terran::Core::Shared<Scene> const& context) override { m_Scene = context; }
    virtual std::string_view GetName() override { return "Scene view"; }

private:
    bool OnKeyPressed(KeyPressedEvent& e);
    void DrawGizmos(glm::mat4 const& projectionMatrix, glm::mat4 const& viewMatrix);

private:
    glm::vec2 m_ViewportSize = { 1080.0f, 790.0f };
    Terran::Core::Shared<SceneRenderer> m_SceneRenderer;

    GizmoType m_GizmoType = GizmoType::None;
    int m_GizmoMode = 1;

    glm::vec2 m_Position = { 0.0f, 0.0f };

    bool m_WindowMoved = false;
    bool m_Visible = false;

    bool m_UseSnapping = false;
    glm::vec3 m_Snap = { 2.0f, 2.0f, 2.0f };

    OpenSceneFN m_OpenSceneCallback;
    std::function<void(glm::vec2)> m_ViewportSizeChangedCallback;
};

}

