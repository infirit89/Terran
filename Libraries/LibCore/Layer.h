#pragma once

#include "Macros.h"
#include "Time.h"
#include "Event.h"

#include <string_view>

namespace Terran {
namespace Core {

class Layer {
public:
    // i dont see a reason currently why a layer should be copyable and/or moveable
    MAKE_NONCOPYABLE(Layer);
    MAKE_NONMOVEABLE(Layer);

    constexpr Layer(std::string_view name) noexcept
        : m_name(name)
    {
    }

    virtual ~Layer() = default;

    // delete the default constructor as a layer should always have a name
    Layer() = delete;

    virtual void update(Time& time) { }
    virtual void on_event(Event& event) { }
    virtual void imgui_render() { }

    // NOTE: maybe make on attach and on dettach to just be the constructor and destructor??
    virtual void on_attach() { }
    virtual void on_dettach() { }

private:
    std::string_view m_name;
};

}

}
