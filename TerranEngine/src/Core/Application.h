#pragma once

#include <string>
#include "LayerStack.h"
#include "Window.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

int main();

namespace TerranEngine {

	class Application
	{
	public:
		Application(std::string);
		virtual ~Application();
		void PushLayer(Layer* layer);

		static Application* Get() { return m_Instance; }
		Window& GetWindow() { return *m_Window; }
	private:
		void Run();
		void OnEvent(Event& event);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		static Application* m_Instance;
		LayerStack m_Stack;
		std::string m_Name;
		friend int ::main();
		std::unique_ptr<Window> m_Window;

		bool m_Minimized = false;
		bool m_Running = true;
	};

	Application* CreateApplication();
}

