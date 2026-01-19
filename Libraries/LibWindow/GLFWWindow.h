#pragma once

#include "Window.h"

#include <LibCore/Base.h>
#include <LibCore/Event.h>

#include <GLFW/glfw3.h>
#include <cstdint>
#include <type_traits>

namespace Terran {
namespace Window {
namespace Implementation {

class GLFWWindow final : public Window {
public:
    GLFWWindow(Core::EventDispatcher& event_dispatcher, WindowData const& data);
    ~GLFWWindow() override;

    void set_vsync(bool enable) override;
    constexpr bool vsync() const noexcept override
    {
        return m_vsync;
    }

    constexpr uint32_t width() const noexcept override
    {
        return m_window_data_ptr.width;
    }

    constexpr uint32_t height() const noexcept override
    {
        return m_window_data_ptr.height;
    }

    void swap_buffers() override;
    void poll_events() override;
    constexpr glm::vec2 content_scale() const noexcept override
    {
        return { m_window_data_ptr.scale_x, m_window_data_ptr.scale_y };
    }

    constexpr void* native_window() const noexcept override
    {
        return m_window;
    }


    void set_title(std::string_view title) override;

private:
    void init(WindowData data);
    void setup_callbacks();
    void destroy();

private:
    struct WindowDataPtr final {
        uint32_t width;
        uint32_t height;
        // content scale
        float scale_x, scale_y;
        bool vsync;
        void const* video_mode;
        Core::RawPtr<Core::EventDispatcher> event_dispatcher;
    };

    bool m_vsync;
    GLFWwindow* m_window = nullptr;
    WindowDataPtr m_window_data_ptr;
};

}
}

}
