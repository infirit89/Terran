#pragma once

#include "Core/Window.h"
#include "GLFW/glfw3.h"

namespace TerranEngine 
{
	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(WindowData data);
		virtual ~GLFWWindow();

		void SetVsync(bool enable) override;
		bool IsVsync() const override { return m_Vsync; }

		int GetWidth() const override { return m_WindowDataPtr.Width; }
		int GetHeight() const override { return m_WindowDataPtr.Height; }

		void Update() override;

		virtual void* GetNativeWindow() { return m_Window; }

		void SetEventCallbackFN(const EventCallbackFn& eventCallbackFN) override { m_WindowDataPtr.EventCallback = eventCallbackFN; }
	private:
		virtual void InitWindow(WindowData data);
		virtual void Close();
	private:
		struct WindowDataPtr 
		{
			uint32_t Width; uint32_t Height;
			bool Vsync;
			EventCallbackFn EventCallback;
		};

		bool m_Vsync;
		GLFWwindow* m_Window;
		WindowDataPtr m_WindowDataPtr;
	};
}