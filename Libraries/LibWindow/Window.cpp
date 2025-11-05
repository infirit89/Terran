#include "Window.h"
#include "GLFWWindow.h"

#include <LibCore/Unique.h>

namespace Terran {
namespace Window {

Core::Unique<Window> Window::create(WindowData const& data)
{
    return Core::Unique<Implementation::GLFWWindow>::create(data);
}

}

}
