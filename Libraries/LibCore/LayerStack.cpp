#include "LayerStack.h"
#include "Layer.h"

#include <algorithm>

namespace Terran {
namespace Core {

LayerStack::~LayerStack()
{
    clear();
}

void LayerStack::push(Layer* layer)
{
    m_layers.emplace_back(layer);
}

void LayerStack::remove(Layer* layer)
{
    auto iterator = std::ranges::find(m_layers, layer);

    layer->on_dettach();
    m_layers.erase(iterator);
    m_lastInsertIndex--;
}

void LayerStack::clear()
{
    while (!m_layers.empty()) {
        Layer* layer = *(m_layers.end() - 1);
        layer->on_dettach();
        delete layer;
        m_layers.erase(m_layers.end() - 1);
    }
}

}

}
