#pragma once

#include "Event.h"

namespace TerranEngine {

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClosed)
	};

	class WindowResizeEvent : public Event 
	{
	public:
		WindowResizeEvent(int width, int height) 
			: m_Width(width), m_Height(height) {}

		EVENT_CLASS_TYPE(WindowResized)

		int GetWidth() { return m_Width; }
		int GetHeight() { return m_Height; }
	private:
		int m_Width; int m_Height;
	};
}