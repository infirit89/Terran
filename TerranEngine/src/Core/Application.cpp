#include "Application.h"
#include "Log.h"

#include <iostream>

namespace TerranEngine 
{
	Application* Application::m_Instance = nullptr;

	Application::Application(std::string name)
		: m_Name(name)
	{
		m_Instance = this;
		m_Window = Window::Create(WindowData(name));
	}
	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_Stack.PushLayer(layer);
	}

	void Application::Run()
	{
		while (true)
		{

			for (Layer* layer : m_Stack.GetLayers()) {
				layer->Update();
			}

			m_Window->Update();
		}
	}
}