#include "SandboxLayer.h"
#include "LibAsset/AssetMetadata.h"
#include "LibCore/Log.h"
#include "LibScene/Components.h"

#include <LibCore/Base.h>
#include <LibCore/Delegate.h>
#include <LibCore/Event.h>
#include <LibCore/Layer.h>
#include <LibCore/UUID.h>

#include <LibGraphics/RendererContext.h>
#include <LibMain/Application.h>
#include <LibScene/Scene.h>
#include <LibScene/SceneSerializer.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <LibWindow/Window.h>
#include <LibWindow/WindowEvent.h>
#include <LibWindow/WindowSystem.h>
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

SandboxLayer::SandboxLayer(Terran::Core::EventDispatcher& event_dispatcher, Terran::Core::RawPtr<Terran::Window::WindowSystem> windowSystem)
    : Layer("Sandbox Layer", event_dispatcher)
    , m_windowSystem(windowSystem)
{
    Terran::Core::EventDispatcher dispatcher;
    Window::WindowData windowData {
        .Name = "Test Window",
    };

    Core::UUID windowId = m_windowSystem->create(windowData);
    auto const& window = m_windowSystem->window(windowId);
    m_renderer_context = new Terran::Graphics::RendererContext("Test app", window);

    event_dispatcher.handlers<Terran::Window::WindowCloseEvent>().connect<&SandboxLayer::on_window_close>(this);
}

Core::Result<void> SandboxLayer::on_attach()
{
    return {};
}

Core::Result<void> SandboxLayer::on_dettach()
{
    delete m_renderer_context;
    return {};
}

void SandboxLayer::update(Terran::Core::Time& time)
{
    m_renderer_context->DrawFrame();
}

void SandboxLayer::imgui_render()
{
    TR_CLIENT_TRACE("Render imgui");
}

void SandboxLayer::on_window_close(Terran::Window::WindowCloseEvent& event)
{
    Terran::Main::Application::get()->close();
}
void SandboxLayer::on_window_resize(Terran::Window::WindowResizeEvent& event)
{
    m_renderer_context->Resize(event.GetWidth(), event.GetHeight());
}

Terran::Core::Result<void> SandboxLayer::on_shutdown()
{
    m_renderer_context->GetLogicalDevice()->WaitIdle();
    return {};
}

}
