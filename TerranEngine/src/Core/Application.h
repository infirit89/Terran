#pragma once

#include "LayerStack.h"
#include "Window.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include <string>
#include <memory>
#include <array>

#include "Log.h"

#include "ImGui/ImGuiLayer.h"

int main();

namespace TerranEngine {

	class Application
	{
	public:
		Application();
		void Create(const WindowData& data);
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
		friend int ::main();
		
	};

	Application* CreateApplication();
}

