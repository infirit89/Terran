#include "trpch.h"

#include "Application.h"
#include "Base.h"
#include "Time.h"
#include "Input.h"

#include "Graphics/Buffer.h"
#include "Graphics/VertexArray.h"		  
#include "Graphics/Shader.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/BatchRenderer2D.h"

#include "Utils/Debug/Profiler.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#pragma warning (push)
#pragma warning (disable : 4244)


namespace TerranEngine 
{
	Application* Application::m_Instance = nullptr;

	static Unique<BatchRenderer2D> s_Renderer;

	Application::Application()
	{
		m_Instance = this;
	}

	void Application::Create(const WindowData& data)
	{
		m_Window = Window::Create(data);

		RenderCommand::Init();
		s_Renderer = CreateUnique<BatchRenderer2D>(20000);

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

		RenderCommand::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		while (m_Running)
		{
			// NOTE: think about changing frametime to be a double
			frameTime = glfwGetTime();
			Time time = Time(frameTime - lastFrameTime);
			lastFrameTime = frameTime;
			
			if (!m_Minimized)
			{
				RenderCommand::Clear();
				for (Layer* layer : m_Stack.GetLayers())
					layer->Update(time);

				m_ImGuiLayer->BeginFrame();
				for (Layer* layer : m_Stack.GetLayers())
					layer->ImGuiRender();
				m_ImGuiLayer->EndFrame();
			}

			m_Window->Update();

			Profiler::Get().ClearResults();
		}
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		// NOTE: should also pause the engine while it's unfocused
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
#pragma warning (pop)
