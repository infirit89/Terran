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

		int GetWidth() const override { return m_Data.Width; }
		int GetHeight() const override { return m_Data.Height; }

		void Update() override;

		virtual void* GetNativeWindow() { return m_Window; }
	private:
		virtual void InitWindow(WindowData data);
		virtual void Close();
	private:
		bool m_Vsync;
		GLFWwindow* m_Window;
		WindowData m_Data;
	};
}