#include "trpch.h"

#include "Application.h"
#include "Base.h"

namespace TerranEngine 
{
	Application* Application::m_Instance = nullptr;

	Application::Application(std::string name)
		: m_Name(name)
	{
		m_Instance = this;
		m_Window = Window::Create(WindowData(name));
		m_Window->SetEventCallbackFN(TR_EVENT_BIND_FN(Application::OnEvent));
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
		while (m_Running)
		{
			if (!m_Minimized)
			{
				for (Layer* layer : m_Stack.GetLayers())
					layer->Update();
			}

			m_Window->Update();
		}
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(TR_EVENT_BIND_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(TR_EVENT_BIND_FN(Application::OnWindowResize));

		for (Layer* layer : m_Stack.GetLayers()) 
		{
			if (event.isHandled)
				break;

			layer->OnEvent(event);
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return false;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) 
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0) 
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;

		return false;
	}
}