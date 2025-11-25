#include "SandboxLayer.h"

#include <LibCore/Base.h>
#include <LibCore/Layer.h>
#include <LibCore/Log.h>
#include <LibCore/Result.h>
#include <LibCore/UUID.h>

#include <LibMain/Application.h>
#include <LibWindow/Window.h>
#include <LibWindow/WindowSystem.h>
#include <LibWindow/WindowEvent.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/projection.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <imgui.h>

#include <math.h>

using namespace Terran;
namespace TerranEngine {

static void TestFunc()
{
    for (size_t i = 0; i < 100; i++)
        TR_CLIENT_TRACE(i * (i + i));

    TR_CLIENT_ERROR("Error: something");

    TR_CLIENT_WARN("Some warning");
}

SandboxLayer::SandboxLayer(Terran::Core::RawPtr<Terran::Window::WindowSystem> windowSystem)
    : Layer("Sandbox Layer")
    , m_windowSystem(windowSystem)
{
    Window::WindowData windowData {
        .Name = "Test Window",
    };

    Core::UUID windowId = m_windowSystem->create(windowData);
    auto const& window = m_windowSystem->window(windowId);
    window->set_event_callback(TR_EVENT_BIND_FN(SandboxLayer::on_event));
}

Core::Result<void> SandboxLayer::on_attach()
{
    return {};
}

Core::Result<void> SandboxLayer::on_dettach()
{
    return {};
}

void SandboxLayer::update(Terran::Core::Time& time)
{
}

void SandboxLayer::on_event(Terran::Core::Event& event)
{
    Terran::Core::EventDispatcher dispather(event);
    dispather.Dispatch<Terran::Window::WindowCloseEvent>(TR_EVENT_BIND_FN(SandboxLayer::on_window_close));
}

void SandboxLayer::imgui_render()
{
}

bool SandboxLayer::on_window_close(Terran::Window::WindowCloseEvent& event)
{
    Terran::Main::Application::get()->close();
    return false;
}

}
