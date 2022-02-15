#pragma once

#include "Events/Event.h"

#include "Base.h"

#include <string>
#include <array>

namespace TerranEngine {

	struct WindowData
	{
		const char* Name;
		uint32_t Width;
		uint32_t Height;
		std::array<std::string, 2> IconPaths = { "", "" };
		bool Fullscren = false;
		bool Maximized = true;
		//bool VSync = true;

		WindowData(const char* name = "Terran Application", uint32_t width = 1080, uint32_t height = 790)
			: Name(name), Width(width), Height(height) {}
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

		virtual void Update() = 0;	

		// NOTE: Using this, because I plan to be able to support other windowing libraries
		static Unique<Window> Create(const WindowData& data = WindowData());

		virtual void SetEventCallbackFN(const EventCallbackFn& eventCallbackFN) = 0;
	};
}