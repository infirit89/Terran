#pragma once

#include <string>
#include "LayerStack.h"
#include "Window.h"

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
	private:
		static Application* m_Instance;
		LayerStack m_Stack;
		std::string m_Name;
		friend int ::main();
		std::unique_ptr<Window> m_Window;

	};

	Application* CreateApplication();
}

