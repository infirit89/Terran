#include "trpch.h"

#include "ImGuiLayer.h"
#include "ImGuiBackEnds.h"

#include "Core/Application.h"

#include "Utils/Debug/OptickProfiler.h"
#include "Graphics/Renderer.h"

#include <imgui.h>

namespace TerranEngine
{
	ImGuiLayer::ImGuiLayer()
		: Layer("Imgui Layer") { }

	ImGuiLayer::~ImGuiLayer() { }

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		auto& app = *Application::Get();
		ImGuiStyle& style = ImGui::GetStyle();
		style.DisplayResizeGrip = false;
		style.AntiAliasedFill = true;
		style.AntiAliasedLines = true;
		style.AntiAliasedLinesUseTex = true;
		
		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow()), true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDettach()
	{
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();

		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		if (m_BlockInput) 
		{
			ImGuiIO io = ImGui::GetIO();
			event.IsHandled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.IsHandled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowContentScaleChangeEvent>(TR_EVENT_BIND_FN(ImGuiLayer::OnWindowContentScaleChangedEvent));
	}

	void ImGuiLayer::BeginFrame()
	{
		TR_PROFILE_FUNCTION();

		Renderer::SubmitCreate([]() 
		{
			ImGui_ImplOpenGL3_NewFrame();
		});

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::EndFrame()
	{
		TR_PROFILE_FUNCTION();
		{
			TR_PROFILE_SCOPE("ImGui::Render");
			ImGui::Render();
		}

		{
			TR_PROFILE_SCOPE("ImGui_ImplOpenGL3_RenderDrawData")
			Renderer::Submit([]()
			{
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			});
		}

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			TR_PROFILE_SCOPE("UpdateImGUIViewports");
			GLFWwindow* context = glfwGetCurrentContext();

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			glfwMakeContextCurrent(context);
		}

	}

	bool ImGuiLayer::OnWindowContentScaleChangedEvent(WindowContentScaleChangeEvent& cscEvent)
	{
		auto& app = *Application::Get();
		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(app.GetWindow().GetContentScale().x);

		return false;
	}
}
