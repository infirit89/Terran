#pragma once

#include "Event.h"

namespace TerranEngine {

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClosed)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowResizeEvent : public Event 
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height) 
			: m_Width(width), m_Height(height) {}

		EVENT_CLASS_TYPE(WindowResized)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		inline uint32_t GetWidth() { return m_Width; }
		inline uint32_t GetHeight() { return m_Height; }
	private:
		uint32_t m_Width; uint32_t m_Height;
	};
}