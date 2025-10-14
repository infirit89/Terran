#pragma once

#include "LibCore/Layer.h"
#include "LibCore/Time.h"
#include "LibCore/Event.h"
#include "Transform.h"

namespace TerranEngine {

class SandboxLayer : public Terran::Core::Layer {
public:
    SandboxLayer();

    void OnAttach() override;
    void OnDetach() override;

    void Update(Terran::Core::Time& time) override;

    void OnEvent(Terran::Core::Event& event) override;
    void ImGuiRender() override;

private:
    Transform m_CameraTransform;
    Transform m_Transform;

    float m_ZoomLevel = 0.1f;
    glm::vec2 m_ViewportSize = { 1080.0f, 790.0f };
};

}
