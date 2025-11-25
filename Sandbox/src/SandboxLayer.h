#pragma once

#include "Transform.h"

#include <LibCore/Base.h>
#include <LibCore/Event.h>
#include <LibCore/Layer.h>
#include <LibCore/Time.h>

#include <LibWindow/WindowEvent.h>
#include <LibWindow/WindowSystem.h>

namespace TerranEngine {

class SandboxLayer : public Terran::Core::Layer {
public:
    SandboxLayer(Terran::Core::RawPtr<Terran::Window::WindowSystem> windowSystem);

    Terran::Core::Result<void> on_attach() override;
    Terran::Core::Result<void> on_dettach() override;

    void update(Terran::Core::Time& time) override;

    void on_event(Terran::Core::Event& event) override;
    bool on_window_close(Terran::Window::WindowCloseEvent& event);
    void imgui_render() override;

private:
    Transform m_CameraTransform;
    Transform m_Transform;

    float m_ZoomLevel = 0.1f;
    glm::vec2 m_ViewportSize = { 1080.0f, 790.0f };
    Terran::Core::RawPtr<Terran::Window::WindowSystem> m_windowSystem;
};

}
