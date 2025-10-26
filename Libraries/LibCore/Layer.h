#pragma once

#include "Event.h"
#include "Macros.h"
#include "Result.h"
#include "Time.h"

#include <string_view>

namespace Terran {
namespace Core {

class Layer {
public:
    // i dont see a reason currently why a layer should be copyable and/or moveable
    MAKE_NONCOPYABLE(Layer);
    MAKE_NONMOVEABLE(Layer);

    virtual ~Layer() = default;

    // delete the default constructor as a layer should always have a name
    Layer() = delete;

    virtual void update(Time& time) { }
    virtual void on_event(Event& event) { }
    virtual void imgui_render() { }

    // NOTE: maybe make on attach and on dettach to just be the constructor and destructor??
    virtual Result<void> on_attach()
    {
        return {};
    }
    virtual Result<void> on_dettach()
    {
        return {};
    }

    constexpr std::string_view name() const noexcept
    {
        return m_name;
    }

protected:
    constexpr Layer(std::string_view name) noexcept
        : m_name(name)
    {
    }

private:
    std::string_view m_name;
};

}

}
