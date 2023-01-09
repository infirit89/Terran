#pragma once

#include "LayerStack.h"
#include "Window.h"
#include "Log.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "ImGui/ImGuiLayer.h"

#include <string>
#include <memory>
#include <array>
#include <filesystem>

int main(int argc, char** argv);

namespace TerranEngine {

    struct ApplicationData
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

		inline static Application* Get() { return m_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		inline ImGuiLayer& GetImGuiLayer() { return *m_ImGuiLayer; }
	private:
		void Run();
		void OnEvent(Event& event);
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

