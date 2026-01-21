/**
 * @file Event.h
 * @brief Event concept and event dispatcher.
 * @ingroup LibCore
 */
#pragma once

#include "Base.h"
#include "Delegate.h"

#include <deque>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Terran::Core {

/**
 * @concept Event
 *
 * @brief For something to classify as an event, it needs to be destructible
 */
template<typename T>
concept Event = std::is_destructible_v<T>;

/**
 * @class EventHandlerBase
 *
 * @brief Declares interface every event handler should follow
 */
class EventHandlerBase {
public:
    virtual void publish() = 0;
    virtual void clear() = 0;
};

/**
 * @class EventHandler
 * @brief Triggers and queues events of a given type
 *
 * @details
 * Implements @see EventHandlerBase, provides:
 * - Connection of event listeners (free functions and memeber functions)
 * - Enqueing and publishing of events of the given type
 * - Immediate dispatching of events
 *
 * @tparam TEvent The event type which will be handled
 */
template<Event TEvent>
class EventHandler final : public EventHandlerBase {
    using event_listener_type = Delegate<void(TEvent&)>;
    using event_listener_container = std::vector<event_listener_type>;
    using event_container = std::deque<TEvent>;

public:
    /**
     * @brief Connects a free function as an even listener
     *
     * @tparam TFunction Free function pointer to bind
     */
    template<auto TFunction>
    void connect()
    {
        event_listener_type listener;
        listener.template connect<TFunction>();
        m_listeners.push_back(std::move(listener));
    }


    /**
     * @brief Connects a member function as an even listener
     *
     * @tparam TFunction Member function pointer to bind
     * @tparam Type Type of the instance to which the member function belongs
     *
     * @param instance Instance on which the member function will be invoked
     *
     * @note The instance is copied into internal storage. No lifetime tracking is performed.
     */
    template<auto TFunction, typename Type>
    void connect(Type instance)
    {
        event_listener_type listener;
        listener.template connect<TFunction, Type>(instance);
        m_listeners.push_back(std::move(listener));
    }

    /**
     * @brief Adds an even that can be published at a later time
     *
     * @param event The event to enqueue
     */
    void enqueue(TEvent const& event)
    {
        m_event_queue.push_back(event);
    }

    /**
     * @brief Immediately disptaches an event to all of the registered event listeners
     *
     * @param event Event to fire
     */
    void trigger(TEvent& event)
    {
        for (auto const& listener : m_listeners) {
            listener(event);
        }
    }

    /**
     * @brief Triggers the registered event listeners for all enqueued events
     *
     * @note Clears the event queue
     */
    void publish() override
    {
        while (!m_event_queue.empty()) {
            TEvent& event = m_event_queue.front();
            trigger(event);
            m_event_queue.pop_front();
        }
    }

    /**
     * @brief Clears the event queue
     */
    void clear() override
    {
        m_event_queue.clear();
    }

private:
    event_container m_event_queue;
    event_listener_container m_listeners;
};

/**
 * @class EventDispatcher
 * @brief Maps event handlers to event types
 */
class EventDispatcher final {

    template<Event TEvent>
    using handler_type = EventHandler<TEvent>;

public:
    /**
     * @brief Gets or creates an event handler for the given event type
     *
     * @tparam TEvent The event type for which to search for or create a handler
     */
    template<Event TEvent>
    [[nodiscard]] handler_type<TEvent>& handlers()
    {
        auto& event_handler_base = m_handlers[typeid(TEvent)];
        if (!event_handler_base) {
            event_handler_base = CreateShared<EventHandler<TEvent>>();
        }
        return static_cast<EventHandler<TEvent>&>(*event_handler_base);
    }

    /**
     * @brief Immediately fires the event to an event handler
     *
     * @tparam TEvent The event type to trigger
     * @param event The event which will be passed to all of the registered listeners
     */
    template<Event TEvent>
    void trigger(TEvent& event) {
       handler_type<TEvent>& handler = handlers<TEvent>();
        handler.trigger(event);
    }

private:
    std::unordered_map<std::type_index, Shared<EventHandlerBase>> m_handlers;
};

}
