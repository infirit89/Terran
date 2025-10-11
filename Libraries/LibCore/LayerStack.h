#pragma once

#include "Layer.h"

#include <vector>

namespace Terran {

namespace Core {

class LayerStack final {
public:
    ~LayerStack();

    void PushLayer(Layer* layer);
    void RemoveLayer(Layer* layer);
    void RemoveAllLayers();

    std::vector<Layer*>& GetLayers() { return m_Layers; }

private:
    std::vector<Layer*> m_Layers;
    int m_LastInsertIndex = 0;
};

}

}
