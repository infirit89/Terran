#pragma once

#include "Layer.h"
#include "Macros.h"
#include "Unique.h"
#include "Result.h"

#include <cstddef>
#include <type_traits>
#include <vector>
#include <utility>

namespace Terran {

namespace Core {

class LayerStack final {
public:
    using layer_container = std::vector<Unique<Layer>>;
    // i dont currently see a reason why layerstack should be copyable or moveable
    MAKE_NONCOPYABLE(LayerStack);
    MAKE_NONMOVEABLE(LayerStack);
    LayerStack() = default;
    ~LayerStack()
    {
        clear();
    }

    template<typename TLayer, typename... Args>
    requires(std::is_base_of_v<Layer, TLayer>)
    Result<void> push(Args... args)
    {
        Unique<Layer> layer = Unique<TLayer>::create(std::forward<Args>(args)...);
        auto result = layer->on_attach();

        if(!result.is_ok())
            return result;

        m_layers.emplace_back(std::move(layer));
    }

    // TODO: error handling for on dettach
    void remove_at(size_t index)
    {
        auto& layer = m_layers.at(index);
        layer->on_dettach();
        m_layers.erase(m_layers.begin() + index);
    }

    // TODO: error handling for on dettach
    void clear()
    {
        while (!m_layers.empty()) {
            auto& layer = m_layers.at(m_layers.size() - 1);
            layer->on_dettach();
            m_layers.erase(m_layers.end() - 1);
        }
    }

    constexpr layer_container const& data() const noexcept
    {
        return m_layers;
    }

    constexpr layer_container::iterator begin() noexcept {
        return m_layers.begin();
    }

    constexpr layer_container::const_iterator begin() const noexcept {
        return m_layers.begin();
    }

    constexpr layer_container::const_iterator cbegin() const noexcept {
        return m_layers.cbegin();
    }

    constexpr layer_container::iterator end() noexcept {
        return m_layers.end();
    }

    constexpr layer_container::const_iterator end() const noexcept {
        return m_layers.end();
    }

    constexpr layer_container::const_iterator cend() const noexcept {
        return m_layers.cend();
    }

private:
    layer_container m_layers;
};

}

}
