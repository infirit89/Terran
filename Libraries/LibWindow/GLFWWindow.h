#pragma once

#include "Window.h"

#include <LibCore/Event.h>

#include <GLFW/glfw3.h>
#include <cstdint>
#include <type_traits>

namespace Terran {
namespace Window {
namespace Implementation {

class GLFWWindow final : public Window {
public:
    GLFWWindow(WindowData const& data);
    ~GLFWWindow() override;

    void set_vsync(bool enable) override;
    constexpr bool vsync() const noexcept override
    {
        return m_Vsync;
    }

    constexpr uint32_t width() const noexcept override
    {
        return m_WindowDataPtr.Width;
    }

    constexpr uint32_t height() const noexcept override
    {
        return m_WindowDataPtr.Height;
    }

    void swap_buffers() override;
    void poll_events() override;
    constexpr glm::vec2 content_scale() const noexcept override
    {
        return { m_WindowDataPtr.XScale, m_WindowDataPtr.YScale };
    }

    constexpr void* native_window() const noexcept override
    {
        return m_Window;
    }

    void set_event_callback(EventCallbackFn const& eventCallbackFN) override;

    void set_title(std::string_view title) override;

private:
    void init(WindowData data);
    void setup_callbacks();
    void destroy();
    template<typename TEvent>
    requires(std::is_base_of_v<Terran::Core::Event, TEvent>)
    static void invokeEventCallback(EventCallbackFn const& eventCallbackFn, TEvent& event) {
        if(eventCallbackFn)
            eventCallbackFn(event);
    }

private:
    struct WindowDataPtr final {
        uint32_t Width;
        uint32_t Height;
        // content scale
        float XScale, YScale;
        bool Vsync;
        EventCallbackFn EventCallback;
        void const* VideoMode;
    };

    bool m_Vsync;
    GLFWwindow* m_Window = nullptr;
    WindowDataPtr m_WindowDataPtr;
};

}
}

}
