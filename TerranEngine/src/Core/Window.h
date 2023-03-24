#pragma once

#include "Base.h"

#include "Events/Event.h"

#include <glm/glm.hpp>

#include <string>
#include <array>

namespace TerranEngine {

	struct WindowData
	{
		const char* Name;
		uint32_t Width = 1280;
		uint32_t Height = 960;
		bool Fullscren = false;
		bool Maximized = true;
		bool VSync = true;
	};
	
	class Window 
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
		virtual ~Window() = default;

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;

		virtual void* GetNativeWindow() = 0;

		virtual void SetVsync(bool enable) = 0;
		virtual bool IsVsync() const = 0;

		virtual void SwapBuffers() = 0;
		virtual void PollEvents() = 0;
		virtual void SetTitle(const char* title) = 0;
		virtual glm::vec2 GetContentScale() = 0;
		
		// NOTE: Using this, because I plan to be able to support other windowing libraries
		static Unique<Window> Create(const WindowData& data = WindowData());

		virtual void SetEventCallbackFN(const EventCallbackFn& eventCallbackFN) = 0;
	};
}
