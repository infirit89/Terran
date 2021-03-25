#include "trpch.h"
#include "ImGuiLayer.h"

#include <imgui.h>

#include "Platform/ImGui/GLFW_OpenGL_Impl/imgui_impl_glfw.h"
#include "Platform/ImGui/GLFW_OpenGL_Impl/imgui_impl_opengl3.h"

#include "Core/Application.h"

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
	}
}
