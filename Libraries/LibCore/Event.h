#pragma once

#include <LibCore/Base.h>
#include <LibCore/Delegate.h>
#include <LibCore/Log.h>
#include <deque>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Terran::Core {

template<typename T>
concept Event = std::is_destructible_v<T>;

class EventHandlerBase {
public:
    virtual void publish() = 0;
    virtual void clear() = 0;
};

template<Event TEvent>
class EventHandler final : public EventHandlerBase {
    using event_listener_type = Delegate<void(TEvent&)>;
    using event_listener_container = std::vector<event_listener_type>;
    using event_container = std::deque<TEvent>;

public:
    template<auto TFunction>
    void connect()
    {
        event_listener_type listener;
        listener.template connect<TFunction>();
        m_listeners.push_back(std::move(listener));
    }

    template<auto TFunction, typename Type>
    void connect(Type instance)
    {
        event_listener_type listener;
        listener.template connect<TFunction, Type>(instance);
        m_listeners.push_back(std::move(listener));
    }

    void enqueue(TEvent const& event)
    {
        m_event_queue.push_back(event);
    }

    void trigger(TEvent& event)
    {
        for (auto const& listener : m_listeners) {
            listener(event);
        }
    }

    void publish()
    {
        while (!m_event_queue.empty()) {
            TEvent& event = m_event_queue.front();
            trigger(event);
            m_event_queue.pop_front();
        }
    }

    void clear()
    {
        m_event_queue.clear();
    }

private:
    event_container m_event_queue;
    event_listener_container m_listeners;
};
class EventDispatcher final {

    template<Event TEvent>
    using handler_type = EventHandler<TEvent>;

public:
    template<Event TEvent>
    [[nodiscard]] handler_type<TEvent>& handlers()
    {
        auto& event_handler_base = m_handlers[typeid(TEvent)];
        if (!event_handler_base) {
            event_handler_base = CreateShared<EventHandler<TEvent>>();
        }
        return static_cast<EventHandler<TEvent>&>(*event_handler_base);
    }

    template<Event TEvent>
    void trigger(TEvent& event) {
       handler_type<TEvent>& handler = handlers<TEvent>();
        handler.trigger(event);
    }

private:
    std::unordered_map<std::type_index, Shared<EventHandlerBase>> m_handlers;
};

}
