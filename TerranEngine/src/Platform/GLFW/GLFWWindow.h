#pragma once

#include "Core/Window.h"

#include "GLFW/glfw3.h"

namespace TerranEngine 
{
	class GLFWWindow final : public Window
	{
	public:
		GLFWWindow(const WindowData& data);
		~GLFWWindow() override;

		void SetVsync(bool enable) override;
		bool IsVsync() const override { return m_Vsync; }

		int GetWidth() const override { return m_WindowDataPtr.Width; }
		int GetHeight() const override { return m_WindowDataPtr.Height; }

		void SwapBuffers() override;
		void PollEvents() override;
		glm::vec2 GetContentScale() override { return { m_WindowDataPtr.XScale, m_WindowDataPtr.YScale }; }

		void* GetNativeWindow() override { return m_Window; }
		
		void SetEventCallbackFN(const EventCallbackFn& eventCallbackFN) override;

		void SetTitle(std::string_view title) override;
	private:
		void InitWindow(WindowData data);
		void SetupCallbacks();
		void Close();

	private:
		struct WindowDataPtr final
		{
			uint32_t Width; uint32_t Height;
			// content scale
			float XScale, YScale;
			bool Vsync;
			EventCallbackFn EventCallback;
			const void* VideoMode;
		};

		bool m_Vsync;
		GLFWwindow* m_Window = nullptr;
		WindowDataPtr m_WindowDataPtr;
	};
}
