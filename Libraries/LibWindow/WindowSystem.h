#pragma once

#include "Window.h"

#include <LibCore/Layer.h>
#include <LibCore/Result.h>
#include <LibCore/Time.h>
#include <LibCore/UUID.h>
#include <LibCore/Unique.h>

#include <LibWindow/ControllerInput.h>
#include <map>

namespace Terran {
namespace Window {

class WindowSystem : public Core::Layer {

public:
    WindowSystem();
    virtual ~WindowSystem() override;

    virtual Core::Result<void> on_attach() override;
    virtual Core::Result<void> on_dettach() override;
    virtual void update(Core::Time& time) override;

    Core::UUID create(WindowData const& data)
    {
        Core::UUID id;
        m_windows.emplace(id, Window::create(data));
        return id;
    }

    constexpr Core::Unique<Window> const& window(Core::UUID const& id) const
    {
        return m_windows.at(id);
    }

    constexpr ControllerInput const& controllers() const
    {
        return m_controllerInput;
    }

private:
    std::map<Core::UUID, Core::Unique<Window>> m_windows;
    ControllerInput m_controllerInput;
};

}
}
