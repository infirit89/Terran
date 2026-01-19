#include "Window.h"
#include "GLFWWindow.h"

#include <LibCore/Event.h>
#include <LibCore/Unique.h>

namespace Terran {
namespace Window {

Core::Unique<Window> Window::create(Core::EventDispatcher& event_dispatcher, WindowData const& data)
{
    return Core::Unique<Implementation::GLFWWindow>::create(event_dispatcher, data);
}

}

}
