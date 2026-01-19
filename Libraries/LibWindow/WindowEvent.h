#pragma once

#include <LibCore/Event.h>

#include <cstdint>

namespace Terran {
namespace Window {

class WindowCloseEvent final {
public:
    WindowCloseEvent() = default;
};

class WindowResizeEvent final {
public:
    WindowResizeEvent(uint32_t width, uint32_t height)
        : m_Width(width)
        , m_Height(height)
    {
    }

    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }

private:
    uint32_t m_Width;
    uint32_t m_Height;
};

class WindowContentScaleChangeEvent final {
public:
    WindowContentScaleChangeEvent(float xScale, float yScale)
        : m_XScale(xScale)
        , m_YScale(yScale)
    {
    }

    float GetXScale() const { return m_XScale; }
    float GetYScale() const { return m_YScale; }

private:
    float m_XScale, m_YScale;
};

}

}
