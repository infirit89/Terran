#include "trpch.h"

#include "Application.h"
#include "Base.h"
#include "Time.h"
#include "Input.h"

#include "Graphics/VertexArray.h"
#include "Graphics/ShaderLibrary.h"
#include "Graphics/Renderer.h"
#include "Graphics/BatchRenderer2D.h"

#include "Scripting/ScriptEngine.h"
//#include "Scripting/ScriptingTest.h"

#include "Physics/Physics.h"

#include "Asset/AssetManager.h"

#include "Utils/Debug/Profiler.h"
#include "Utils/Debug/OptickProfiler.h"

#include <GLFW/glfw3.h>

#pragma warning (push)
#pragma warning (disable : 4244)

namespace TerranEngine
{
	Application* Application::m_Instance = nullptr;

	Application::Application(const ApplicationData& appData)
	{
		m_Instance = this;

		m_Window = Window::Create(appData.Window);

		Renderer::Init();
		BatchRenderer2D::Initialize(2000);
        
		// TODO: this should NOT be initialized here; 
		// it should be initialized on project load
		AssetManager::Init();
        ScriptEngine::Initialize(appData.ScriptCorePath);
		//ScriptingTest::Initialize();
        Physics2D::Initialize();

		m_Window->SetEventCallbackFN(TR_EVENT_BIND_FN(Application::OnEvent));

		Input::Init();
		
		m_ImGuiLayer = new ImGuiLayer();
		PushLayer(m_ImGuiLayer);
	}

	Application::~Application()
	{
		m_Stack.RemoveAllLayers();
        ScriptEngine::Shutdown();
		//ScriptingTest::Shutdown();
        Physics2D::Shutdown();
		BatchRenderer2D::Shutdown();
		AssetManager::Shutdown();
		Renderer::Shutdown();
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

		Renderer::SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		while (m_Running)
		{
			TR_PROFILE_FRAME("MainThread");
			// NOTE: think about changing frametime to be a double
			frameTime = glfwGetTime();
			Time time(frameTime - lastFrameTime);
			lastFrameTime = frameTime;
			
			if (!m_Minimized)
			{
				Renderer::Clear();
				{
					TR_PROFILE_SCOPE("Layer::OnUpdate");
					for (Layer* layer : m_Stack.GetLayers())
						layer->Update(time);
				}

				m_ImGuiLayer->BeginFrame(); 
				{
					TR_PROFILE_SCOPE("Layer::ImGuiRender");
					for (Layer* layer : m_Stack.GetLayers())
						layer->ImGuiRender();
				}
				m_ImGuiLayer->EndFrame();

				Renderer::ExecuteCommands();
				m_Window->SwapBuffers();
			}

			m_Window->PollEvents();
			Input::Update();

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
			if (event.IsHandled)
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

		Renderer::SetViewport(e.GetWidth(), e.GetHeight());

		return false;
	}
}
#pragma warning (pop)
