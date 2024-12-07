#pragma once

#include <functional>

namespace TerranEngine 
{
	enum class EventType : uint32_t
	{
		None = 0,
		KeyPressed, KeyReleased,
		MouseMoved, MouseScrolled,
		MouseButtonPressed, MouseButtonReleased,
		WindowClosed, WindowResized, WindowContentScaleChanged,
		GamePadConnected, GamePadDisconnected,
		SceneTransitionEvent,
		CustomEvent
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = 1 << 0,
		EventCategoryKeyboard = 1 << 1,
		EventCategoryMouse = 1 << 2,
		EventCategoryGamePad = 1 << 3
	};

	class Event 
	{
	public:
		virtual ~Event() = default;

		virtual EventType GetType() const = 0;
		virtual EventCategory GetCategory() const = 0;

		bool IsInCategory(EventCategory category) const { return GetCategory() & category; }

		bool IsHandled = false;
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
virtual EventType GetType() const override { return GetStaticType(); }

#define EVENT_CLASS_CATEGORY(category) virtual EventCategory GetCategory() const override { return category; }

	class EventDispatcher final
	{
		template<typename T>
		using EventFN = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{}

		template<typename T>
		bool Dispatch(const EventFN<T>& func) 
		{
			if (m_Event.GetType() == T::GetStaticType())
			{
				if (!m_Event.IsHandled) 
				{
					m_Event.IsHandled |= func(static_cast<T&>(m_Event));
					return true;
				}

				return false;
			}

			return false;
		}
	private:
		Event& m_Event;

	};
}
