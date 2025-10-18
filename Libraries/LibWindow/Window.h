#pragma once

#include "LibCore/Base.h"
#include "LibCore/Event.h"

#include <glm/glm.hpp>

#include <cstdint>
#include <string>
#include <string_view>

namespace TerranEngine {

struct WindowData final {
    WindowData() = default;
    ~WindowData() = default;

    std::string_view Name = "";
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
    virtual ~Window() = default;

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;

    virtual void* GetNativeWindow() = 0;

    virtual void SetVsync(bool enable) = 0;
    virtual bool IsVsync() const = 0;

    virtual void SwapBuffers() = 0;
    virtual void PollEvents() = 0;
    virtual void SetTitle(std::string_view) = 0;
    virtual glm::vec2 GetContentScale() = 0;

    // NOTE: Using this, because I plan to be able to support other windowing libraries
    static Terran::Core::Unique<Window> Create(WindowData const& data = WindowData());

    virtual void SetEventCallbackFN(EventCallbackFn const& eventCallbackFN) = 0;
};

}
