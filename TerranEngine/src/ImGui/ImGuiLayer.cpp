#include "trpch.h"
#include "ImGuiLayer.h"

#include "ImGuiBackEnds.h"

#include "Core/Application.h"

#include <imgui.h>

namespace TerranEngine 
{
	ImGuiLayer::ImGuiLayer()
		: Layer("Imgui Layer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		
		ImGuiStyle& style = ImGui::GetStyle();
		style.DisplayResizeGrip = false;
		style.AntiAliasedFill = true;
		style.AntiAliasedLines = true;
		style.AntiAliasedLinesUseTex = true;

		ImGui::StyleColorsDark();

		auto& app = *Application::Get();
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
			event.isHandled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.isHandled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::BeginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::EndFrame()
	{

		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* context = glfwGetCurrentContext();

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();

			glfwMakeContextCurrent(context);
		}

	}
}
