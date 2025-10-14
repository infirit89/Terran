/**
 * @file Application.cpp
 * @brief Implementation of the core Application class
 */

#include "Application.h"
#include "LibCore/Assert.h"
#include "LibCore/Time.h"

namespace Terran {
namespace Main {

Application* Application::m_Instance = nullptr;

Application::Application() noexcept
{
    TR_ASSERT(!m_Instance, "There is already another instance of the application");
    m_Instance = this;
}

Application::~Application()
{
    m_Stack.RemoveAllLayers();
}

void Application::push_layer(Terran::Core::Layer* layer)
{
    m_Stack.PushLayer(layer);
    layer->OnAttach();
}

void Application::close()
{
    m_Running = false;
}

void Application::dispatch_event(Terran::Core::Event& event)
{
    // TODO: Implement event dispatching system
}

void Application::run()
{
    TR_ASSERT(!m_Instance->m_Running, "The application is already running");
    
    m_Running = true;
    float lastFrameTime = 0.0f;

    while (m_Running) {
        // TODO: Re-enable profiling when system is stabilized
        // TR_PROFILE_FRAME("MainThread");
        
        // TODO: Implement proper frame timing
        // NOTE: Consider changing frametime to be a double for better precision
        // float frameTime = glfwGetTime();
        // Terran::Core::Time time(frameTime - lastFrameTime);
        // lastFrameTime = frameTime;
        
        Terran::Core::Time time(0.0f);

        if (!m_Minimized) {
            {
                // TODO: Re-enable profiling scope
                // TR_PROFILE_SCOPE("Layer::OnUpdate");
                for (Terran::Core::Layer* layer : m_Stack.GetLayers())
                    layer->Update(time);
            }

            // TODO: Re-enable ImGui rendering when ImGui layer is restored
            // m_ImGuiLayer->BeginFrame();
            // {
            //     TR_PROFILE_SCOPE("Layer::ImGuiRender");
            //     for (Terran::Core::Layer* layer : m_Stack.GetLayers())
            //         layer->ImGuiRender();
            // }
            // m_ImGuiLayer->EndFrame();
        }
    }
}

// void Application::DispatchEvent(Terran::Core::Event& event)
// {
//     Terran::Core::EventDispatcher dispatcher(event);
//
//     // NOTE: should also pause the engine while it's unfocused
//     dispatcher.Dispatch<WindowCloseEvent>(TR_EVENT_BIND_FN(Application::OnWindowClose));
//     dispatcher.Dispatch<WindowResizeEvent>(TR_EVENT_BIND_FN(Application::OnWindowResize));
//
//     for (Terran::Core::Layer* layer : m_Stack.GetLayers()) {
//         if (event.IsHandled)
//             break;
//
//         layer->OnEvent(event);
//     }
// }

// bool Application::OnWindowClose(WindowCloseEvent& e)
// {
//     m_Running = false;
//     return false;
// }

// bool Application::OnWindowResize(WindowResizeEvent& e)
// {
//     if (e.GetWidth() == 0 || e.GetHeight() == 0) {
//         m_Minimized = true;
//         return false;
//     }
//
//     m_Minimized = false;
//
//     Renderer::SetViewport(e.GetWidth(), e.GetHeight());
//
//     return false;
// }
//

}

}
