#pragma once

#include "Core/Window.h"

#include "GLFW/glfw3.h"

namespace TerranEngine 
{
	class GLFWWindow final : public Window
	{
	public:
		GLFWWindow(WindowData data);
		virtual ~GLFWWindow();

		void SetVsync(bool enable) override;
		inline bool IsVsync() const override { return m_Vsync; }

		inline int GetWidth() const override { return m_WindowDataPtr.Width; }
		inline int GetHeight() const override { return m_WindowDataPtr.Height; }

		virtual void SwapBuffers() override;
		virtual void PollEvents() override;
		virtual glm::vec2 GetContentScale() override { return { m_WindowDataPtr.XScale, m_WindowDataPtr.YScale }; }
		
		virtual void* GetNativeWindow() { return m_Window; }
		
		void SetEventCallbackFN(const EventCallbackFn& eventCallbackFN) override;

		void SetTitle(const char* title) override;
	private:
		void InitWindow(WindowData data);
		void SetupCallbacks();
		void Close();
	private:

		struct WindowDataPtr 
		{
			uint32_t Width; uint32_t Height;
			// content scale
			float XScale, YScale;
			bool Vsync;
			EventCallbackFn EventCallback;
			const void* VideoMode;
		};

		bool m_Vsync;
		GLFWwindow* m_Window = NULL;
		WindowDataPtr m_WindowDataPtr;
	};
}
