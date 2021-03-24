#pragma once

#include <functional>

namespace TerranEngine 
{
	enum class EventType
	{
		None = 0,
		KeyPressed, KeyReleased,
		MouseMoved, MouseScrolled,
		MouseButtonPressed, MouseButtonReleased,
		WindowClosed, WindowResized
	};

	class Event 
	{
	public:
		~Event() = default;

		virtual EventType GetType() const = 0;
		bool isHandled = false;
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
virtual EventType GetType() const override { return GetStaticType(); }

	class EventDispatcher 
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
				m_Event.isHandled |= func((T&)m_Event);
				return true;
			}

			return false;
		}
	private:
		Event& m_Event;

	};
}

