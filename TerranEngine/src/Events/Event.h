#pragma once

#include <functional>

#pragma warning (push)
#pragma warning (disable : 26812) 

namespace TerranEngine 
{
	enum class EventType
	{
		None = 0,
		KeyPressed, KeyReleased,
		MouseMoved, MouseScrolled,
		MouseButtonPressed, MouseButtonReleased,
		WindowClosed, WindowResized,
		GamepadConnected, GamepadDisconnected
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = 1 << 0,
		EventCategoryKeyboard = 1 << 1,
		EventCategoryMouse = 1 << 2,
		EventCategoryGamepad = 1 << 3
	};

	class Event 
	{
	public:
		~Event() = default;

		virtual EventType GetType() const = 0;
		virtual EventCategory GetCategory() const = 0;

		inline bool IsInCategory(EventCategory category) { return GetCategory() & category; }

		bool IsHandled = false;
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
virtual EventType GetType() const override { return GetStaticType(); }

#define EVENT_CLASS_CATEGORY(category) virtual EventCategory GetCategory() const override { return category; }

	class EventDispatcher 
	{
		template<typename T>
		using EventFN = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{}

		template<typename T>
		inline bool Dispatch(const EventFN<T>& func) 
		{
			if (m_Event.GetType() == T::GetStaticType())
			{
				if (!m_Event.IsHandled) 
				{
					m_Event.IsHandled |= func((T&)m_Event);
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

#pragma warning (pop)
