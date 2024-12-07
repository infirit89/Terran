#pragma once

#include "LayerStack.h"
#include "Window.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "ImGui/ImGuiLayer.h"

#include <memory>
#include <filesystem>

int main(int argc, char** argv);

namespace TerranEngine {

    struct ApplicationData final
    {
        WindowData Window;
        std::filesystem::path ScriptCorePath;
		std::filesystem::path ProjectPath;
    };

	class Application
	{
	public:
		Application(const ApplicationData& appData);
		virtual ~Application();
		void PushLayer(Layer* layer);
		void Close();
		void DispatchEvent(Event& event);
		
		static Application* Get() { return m_Instance; }
		Window& GetWindow() const { return *m_Window; }
		ImGuiLayer& GetImGuiLayer() const { return *m_ImGuiLayer; }

	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		static Application* m_Instance;
		LayerStack m_Stack;

		std::unique_ptr<Window> m_Window;
		
		bool m_Minimized = false;
		bool m_Running = true;
		
		ImGuiLayer* m_ImGuiLayer;
		friend int ::main(int argc, char** argv);
		
	};

	Application* CreateApplication(int argc, char** argv);
}

