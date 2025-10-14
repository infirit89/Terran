#pragma once

#include "Window.h"

#include "Events/ApplicationEvent.h"

#include "ImGui/ImGuiLayer.h"

#include "LibCore/Event.h"
#include "LibCore/Layer.h"
#include "LibCore/LayerStack.h"

#include <filesystem>
#include <memory>

int main(int argc, char** argv);

namespace TerranEngine {

struct ApplicationData final {
};

class Application {
public:
    Application(ApplicationData const& appData);
    virtual ~Application();
    void PushLayer(Terran::Core::Layer* layer);
    void Close();
    void DispatchEvent(Terran::Core::Event& event);

    static Application* Get() { return m_Instance; }

private:
    void Run();
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

    static Application* m_Instance;
    Terran::Core::LayerStack m_Stack;

    bool m_Minimized = false;
    bool m_Running = true;

    friend int ::main(int argc, char** argv);
};

Application* CreateApplication(int argc, char** argv);

}
