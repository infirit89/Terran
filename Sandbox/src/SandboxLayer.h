#pragma once

#include "Terran.h"

#include "Transform.h"
#include <memory>

namespace TerranEngine {

class SandboxLayer : public Terran::Core::Layer {
public:
    SandboxLayer();

    void OnAttach() override;
    void OnDetach() override;

    void Update(Terran::Core::Time& time) override;

    void OnEvent(Terran::Core::Event& event) override;
    void ImGuiRender() override;

    bool KeyPressed(KeyPressedEvent& event);
    bool OnWindowResize(WindowResizeEvent& event);
    bool OnMouseScroll(MouseScrollEvent& event);

private:
    OrthographicCamera m_Camera;
    Transform m_CameraTransform;
    Transform m_Transform;

    float m_ZoomLevel = 0.1f;
    glm::vec2 m_ViewportSize = { 1080.0f, 790.0f };

    Terran::Core::Shared<Framebuffer> m_Framebuffer;
};

}
