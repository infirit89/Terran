/**
 * @file Layer.h
 * @brief Base for engine and user-defined systems
 * @ingroup LibCore
 */
#pragma once

#include "Event.h"
#include "Macros.h"
#include "Result.h"
#include "Time.h"

#include <string_view>

namespace Terran::Core {

/**
 * @class Layer
 * @brief Base class for every engine and user-defined system
 */
class Layer {
public:
    // i dont see a reason currently why a layer should be copyable and/or moveable
    MAKE_NONCOPYABLE(Layer);
    MAKE_NONMOVEABLE(Layer);

    virtual ~Layer() = default;

    // delete the default constructor as a layer should always have a name
    Layer() = delete;

    /**
     * @brief Called every update cycle
     *
     * @param time The timestep from the previous update
     */
    virtual void update(Time& time) { }

    virtual void imgui_render() { }

    // NOTE: maybe make on attach and on dettach to just be the constructor and destructor??
    /**
     * @brief Called when the layer is constructed
     */
    virtual Result<void> on_attach()
    {
        return {};
    }

    /**
     * @brief Called when the layer is destructed
     */
    virtual Result<void> on_dettach()
    {
        return {};
    }

    constexpr std::string_view name() const noexcept
    {
        return m_name;
    }

protected:
    constexpr Layer(std::string_view name, EventDispatcher& dispatcher) noexcept
        : m_name(name), event_dispatcher(dispatcher)
    {
    }

    EventDispatcher& event_dispatcher;

private:
    std::string_view m_name;
};

}
