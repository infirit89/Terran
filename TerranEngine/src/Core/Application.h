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
		Application(std::string name, std::array<std::string, 2> iconPaths);
		virtual ~Application();
		void PushLayer(Layer* layer);
		void Close();

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
		std::array<std::string, 2> m_IconPaths;

		std::unique_ptr<Window> m_Window;

		bool m_Minimized = false;
		bool m_Running = true;

		unsigned int m_Vao, m_Vbo, m_Ibo;

		ImGuiLayer* m_ImGuiLayer;
		friend int ::main();
	};

	Application* CreateApplication();
}

