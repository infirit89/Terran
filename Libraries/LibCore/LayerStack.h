/**
 * @file LayerStack.h
 * @brief Layer container.
 * @ingroup LibCore
 */
#pragma once

#include "Layer.h"
#include "Macros.h"
#include "Result.h"
#include "Unique.h"
#include "Base.h"
#include "Event.h"

#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

namespace Terran::Core {

/**
 * @class LayerStack
 * @brief Layer container that handles the creation and deletion of layers
 */
class LayerStack final {
public:
    using layer_container = std::vector<Unique<Layer>>;
    // i dont currently see a reason why layerstack should be copyable or moveable
    MAKE_NONCOPYABLE(LayerStack);
    MAKE_NONMOVEABLE(LayerStack);
    LayerStack(RawPtr<EventDispatcher> dispatcher)
        : m_dispatcher(dispatcher)
    {
    }
    ~LayerStack()
    {
        clear();
    }

    /**
     * @brief Create a layer with the given arguments
     *
     * @details Additinally this includes the even dispatcher as a first argument, thus layer implementations should always take an @see EventDispatcher as a first argument in their constructors
     *
     * @param args The arguments passed to the constructor of the layer
     */
    template<typename TLayer, typename... Args>
    requires(std::is_base_of_v<Layer, TLayer>)
    Result<void> push(Args... args)
    {
        Unique<Layer> layer = Unique<TLayer>::create(*m_dispatcher, std::forward<Args>(args)...);
        auto result = layer->on_attach();

        if (!result.is_ok())
            return result;

        m_layers.emplace_back(std::move(layer));
        return {};
    }

    // TODO: error handling for on dettach
    /**
     * @brief Removes a layer and calls its on_detach method
     *
     * @param index The index at which to remove a layer
     */
    void remove_at(size_t index)
    {
        auto& layer = m_layers.at(index);
        layer->on_dettach();
        m_layers.erase(m_layers.begin() + index);
    }

    template<typename TLayer>
    requires(std::is_base_of_v<Layer, TLayer>)
    /**
     * @brief Get a layer based on its type
     * @retuns View into a unique pointer, DO NOT DELETE
     */
    RawPtr<TLayer> get()
    {
        for (auto const& layer : m_layers) {
            if (auto casted_layer = dynamic_cast<RawPtr<TLayer>>(layer.data())) {
                return casted_layer;
            }
        }

        return nullptr;
    }

    // TODO: error handling for on dettach
    /**
     * @brief Clears all the layers and calls their on_detach method
     */
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

    constexpr layer_container::iterator begin() noexcept
    {
        return m_layers.begin();
    }

    constexpr layer_container::const_iterator begin() const noexcept
    {
        return m_layers.begin();
    }

    constexpr layer_container::const_iterator cbegin() const noexcept
    {
        return m_layers.cbegin();
    }

    constexpr layer_container::iterator end() noexcept
    {
        return m_layers.end();
    }

    constexpr layer_container::const_iterator end() const noexcept
    {
        return m_layers.end();
    }

    constexpr layer_container::const_iterator cend() const noexcept
    {
        return m_layers.cend();
    }

private:
    layer_container m_layers;
    RawPtr<EventDispatcher> m_dispatcher;
};

}

