#include "trpch.h"

#include "Application.h"
#include "LibCore/Time.h"

#pragma warning(push)
#pragma warning(disable : 4244)

namespace TerranEngine {

Application* Application::m_Instance = nullptr;

Application::Application(ApplicationData const& appData)
{
    m_Instance = this;
}

Application::~Application()
{
    m_Stack.RemoveAllLayers();
}

void Application::PushLayer(Terran::Core::Layer* layer)
{
    m_Stack.PushLayer(layer);
    layer->OnAttach();
}

void Application::Close()
{
    m_Running = false;
}

void Application::Run()
{
    float lastFrameTime = 0.0f;

    while (m_Running) {
        // TR_PROFILE_FRAME("MainThread");
        // NOTE: think about changing frametime to be a double
        // float frameTime = glfwGetTime();
        // Terran::Core::Time time(frameTime - lastFrameTime);
        // lastFrameTime = frameTime;
        Terran::Core::Time time(0.0f);

        if (!m_Minimized) {
            {
                // TR_PROFILE_SCOPE("Layer::OnUpdate");
                for (Terran::Core::Layer* layer : m_Stack.GetLayers())
                    layer->Update(time);
            }

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
#pragma warning(pop)
