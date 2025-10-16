#pragma once

#include "LibCore/Layer.h"
#include "LibCore/Time.h"
#include "LibCore/Event.h"
#include "Transform.h"

namespace TerranEngine {

class SandboxLayer : public Terran::Core::Layer {
public:
    SandboxLayer();

    void on_attach() override;
    void on_dettach() override;

    void update(Terran::Core::Time& time) override;

    void on_event(Terran::Core::Event& event) override;
    void imgui_render() override;

private:
    Transform m_CameraTransform;
    Transform m_Transform;

    float m_ZoomLevel = 0.1f;
    glm::vec2 m_ViewportSize = { 1080.0f, 790.0f };
};

}
