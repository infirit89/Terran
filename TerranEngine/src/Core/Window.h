#pragma once

#include "Events/Event.h"

#include <string>
#include <memory>

namespace TerranEngine {

	struct WindowData
	{
		std::string Name;
		uint32_t Width;
		uint32_t Height;

		WindowData(std::string name = "Terran Application", uint32_t width = 1080, uint32_t height = 790)
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

		static std::unique_ptr<Window> Create(const WindowData& data = WindowData());

		virtual void SetEventCallbackFN(const EventCallbackFn& eventCallbackFN) = 0;
	};
}