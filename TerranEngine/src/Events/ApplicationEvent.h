#pragma once

#include "Event.h"

namespace TerranEngine {

	class WindowCloseEvent final : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClosed)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowResizeEvent final : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height) 
			: m_Width(width), m_Height(height) {}

		EVENT_CLASS_TYPE(WindowResized)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
	private:
		uint32_t m_Width; uint32_t m_Height;
	};

	class WindowContentScaleChangeEvent final : public Event
	{
	public:
		WindowContentScaleChangeEvent(float xScale, float yScale) 
			: m_XScale(xScale), m_YScale(yScale) {}

		EVENT_CLASS_TYPE(WindowContentScaleChanged)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		float GetXScale() const { return m_XScale; }
		float GetYScale() const { return m_YScale; }

	private:
		float m_XScale, m_YScale;
	};
}