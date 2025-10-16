#pragma once

#include "Layer.h"
#include "Macros.h"

#include <vector>

namespace Terran {

namespace Core {

class LayerStack final {
public:
    // i dont currently see a reason why layerstack should be copyable or moveable
    MAKE_NONCOPYABLE(LayerStack);
    MAKE_NONMOVEABLE(LayerStack);
    LayerStack() = default;
    ~LayerStack();

    void push(Layer* layer);
    void remove(Layer* layer);
    void clear();

    constexpr std::vector<Layer*> const& layers() const noexcept
    {
        return m_layers;
    }

private:
    std::vector<Layer*> m_layers;
    int m_lastInsertIndex = 0;
};

}

}
