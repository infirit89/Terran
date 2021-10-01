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
		inline bool IsVsync() const override { return m_Vsync; }

		inline int GetWidth() const override { return m_WindowDataPtr.Width; }
		inline int GetHeight() const override { return m_WindowDataPtr.Height; }

		void Update() override;
		
		virtual void* GetNativeWindow() { return m_Window; }

		inline void SetEventCallbackFN(const EventCallbackFn& eventCallbackFN) override { m_WindowDataPtr.EventCallback = eventCallbackFN; }
	private:
		void InitWindow(WindowData data);
		void Close();
	private:

		struct WindowDataPtr 
		{
			uint32_t Width; uint32_t Height;
			bool Vsync;
			EventCallbackFn EventCallback;
		};

		bool m_Vsync;
		GLFWwindow* m_Window = NULL;
		WindowDataPtr m_WindowDataPtr;
	};
}