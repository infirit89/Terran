#include "LayerStack.h"
#include "Layer.h"

#include <algorithm>

namespace Terran {
namespace Core {

LayerStack::~LayerStack()
{
    RemoveAllLayers();
}

void LayerStack::PushLayer(Layer* layer)
{
    m_Layers.emplace_back(layer);
}

void LayerStack::RemoveLayer(Layer* layer)
{
    auto iterator = std::ranges::find(m_Layers, layer);

    layer->OnDetach();
    m_Layers.erase(iterator);
    m_LastInsertIndex--;
}

void LayerStack::RemoveAllLayers()
{
    while (!m_Layers.empty()) {
        Layer* layer = *(m_Layers.end() - 1);
        layer->OnDetach();
        delete layer;
        m_Layers.erase(m_Layers.end() - 1);
    }
}

}

}
