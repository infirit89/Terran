/**
 * @file Unique.h
 * @brief Unique pointer.
 * @ingroup LibCore
 */
#pragma once

#include "DefaultDelete.h"
#include "Macros.h"

#include <type_traits>
#include <utility>

namespace Terran::Core {

// An owning pointer
/**
 * @class Unique
 * @brief Noncopyable owning smart pointer, used for memory management
 */
template<typename T, typename TDeleter = DefaultDelete<T>>
class Unique {
public:
    using deleter_type = TDeleter;
    MAKE_NONCOPYABLE(Unique);

    constexpr Unique() noexcept = default;

    ~Unique() noexcept
    {
        reset();
    }

    constexpr Unique(T* data) noexcept
        : m_data(data)
    {
    }

    constexpr Unique(Unique&& other) noexcept
    {
        m_data = other.m_data;
        other.m_data = nullptr;
    }

    constexpr Unique& operator=(Unique&& other) noexcept
    {
        m_data = other.m_data;
        other.m_data = nullptr;
        return *this;
    }

    template<typename U>
    constexpr Unique(Unique<U>&& other) noexcept
    requires(std::is_convertible_v<U*, T*>)
        : m_data(other.release())
    {
    }

    constexpr T* data() const noexcept
    {
        return m_data;
    }

    constexpr T* operator->() const noexcept
    {
        return m_data;
    }

    [[nodiscard]] constexpr T* release() noexcept
    {
        T* released_data = m_data;
        m_data = nullptr;
        return released_data;
    }

    constexpr void reset() noexcept
    {
        auto* data = std::exchange(m_data, nullptr);
        m_deleter(data);
    }

    [[nodiscard]] constexpr deleter_type deleter() noexcept
    {
        return m_deleter;
    }

    template<typename... Args>
    constexpr static Unique create(Args&&... args)
    {
        return Unique(new T(std::forward<Args>(args)...));
    }

private:
    T* m_data = nullptr;
    deleter_type m_deleter {};
};

}
