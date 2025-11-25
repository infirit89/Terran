#pragma once

#include <cstdint>
#include <functional>
#include <type_traits>

namespace Terran {
namespace Core {

enum class EventType : std::uint8_t {
    None = 0,
    KeyPressed,
    KeyReleased,
    MouseMoved,
    MouseScrolled,
    MouseButtonPressed,
    MouseButtonReleased,
    WindowClosed,
    WindowResized,
    WindowContentScaleChanged,
    GamePadConnected,
    GamePadDisconnected,
    SceneTransitionEvent,
    CustomEvent
};

enum EventCategory : std::uint8_t {
    None = 0,
    EventCategoryApplication = 1 << 0,
    EventCategoryKeyboard = 1 << 1,
    EventCategoryMouse = 1 << 2,
    EventCategoryGamePad = 1 << 3
};

class Event {
public:
    virtual ~Event() = default;

    virtual EventType GetType() const = 0;
    virtual EventCategory GetCategory() const = 0;

    bool IsInCategory(EventCategory category) const { return GetCategory() & category; }

    bool IsHandled = false;
};

#define EVENT_CLASS_TYPE(type)                                   \
    static Terran::Core::EventType GetStaticType() { return Terran::Core::EventType::type; } \
    virtual Terran::Core::EventType GetType() const override { return GetStaticType(); }

#define EVENT_CLASS_CATEGORY(category) \
    virtual Terran::Core::EventCategory GetCategory() const override { return Terran::Core::category; }

class EventDispatcher final {
    template<typename TEvent>
    using EventFN = std::function<bool(TEvent&)>;

public:
    EventDispatcher(Event& event)
        : m_Event(event)
    {
    }

    template<typename TEvent>
    requires(std::is_base_of_v<Event, TEvent>)
    bool Dispatch(EventFN<TEvent> const& func)
    {
        if (m_Event.GetType() == TEvent::GetStaticType()) {
            if (!m_Event.IsHandled) {
                m_Event.IsHandled |= func(static_cast<TEvent&>(m_Event));
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

}
