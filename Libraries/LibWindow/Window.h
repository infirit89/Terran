#pragma once

#include <LibCore/Event.h>
#include <LibCore/Macros.h>
#include <LibCore/Unique.h>

#include <glm/glm.hpp>

#include <cstdint>
#include <string_view>
#include <functional>

namespace Terran {
namespace Window {

struct WindowData final {
    WindowData() = default;
    ~WindowData() = default;

    std::string_view Name;
    uint32_t Width = 1280;
    uint32_t Height = 960;
    bool Fullscreen = false;
    bool Maximized = true;
    bool VSync = true;
    bool Debug = false;
};

class Window {
public:
    using EventCallbackFn = std::function<void(Terran::Core::Event&)>;

    // NOTE: i dont see a reason currently why the window should be copyable or movable
    MAKE_NONCOPYABLE(Window);
    MAKE_NONMOVEABLE(Window);

    virtual ~Window() = default;

    virtual constexpr uint32_t width() const noexcept = 0;
    virtual constexpr uint32_t height() const noexcept = 0;

    virtual constexpr void* native_window() const noexcept = 0;

    virtual void set_vsync(bool enable) = 0;
    virtual constexpr bool vsync() const noexcept = 0;

    virtual void swap_buffers() = 0;
    virtual void poll_events() = 0;
    virtual void set_title(std::string_view) = 0;
    virtual constexpr glm::vec2 content_scale() const noexcept = 0;

    // NOTE: Using this, because I plan to be able to support other windowing libraries
    static Terran::Core::Unique<Window> create(WindowData const& data = WindowData());

    virtual void set_event_callback(EventCallbackFn const& eventCallbackFN) = 0;
};

}

}
