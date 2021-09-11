#include "trpch.h"

#include "Application.h"
#include "Base.h"
#include "Time.h"

#include <GLFW/glfw3.h>

#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"

#include "Renderer/Shader.h"
#include "Renderer/RenderCommand.h"

namespace TerranEngine 
{
	Application* Application::m_Instance = nullptr;

	Application::Application(const char* name, std::array<std::string, 2> iconPaths)
		: m_Name(name), m_IconPaths(iconPaths)
	{
		m_Instance = this;
		m_Window = Window::Create(WindowData(name, 1080, 790, m_IconPaths));
		RenderCommand::Init();

		m_Window->SetEventCallbackFN(TR_EVENT_BIND_FN(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushLayer(m_ImGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
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
		float frameTime = 0.0f; float lastFrameTime = 0.0f;

		while (m_Running)
		{

			frameTime = glfwGetTime();
			float dt(frameTime - lastFrameTime);
			lastFrameTime = frameTime;
			

			if (!m_Minimized)
			{
				for (Layer* layer : m_Stack.GetLayers())
					layer->Update(dt);

				m_ImGuiLayer->BeginFrame();
				for (Layer* layer : m_Stack.GetLayers())
					layer->ImGuiRender();
				m_ImGuiLayer->EndFrame();
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

		RenderCommand::Resize(e.GetWidth(), e.GetHeight());

		return false;
	}
}
