#pragma once

#include "DefaultDelete.h"
#include <utility>
namespace Terran {
namespace Core {

// An owning pointer
template<typename T, typename TDeleter = DefaultDelete<T>>
class Unique {
public:
    constexpr Unique() noexcept = default;

    ~Unique() noexcept
    {
        TDeleter m_data;
        m_data = nullptr;
    }

    constexpr Unique(T* data) noexcept
        : m_data(data)
    {
    }

    // Make non copyable
    Unique(Unique const& other) = delete;
    Unique& operator=(Unique const& other) = delete;

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

    constexpr T* data() noexcept
    {
        return m_data;
    }

    constexpr T& operator->() noexcept
    {
        return &m_data;
    }

    template<typename... Args>
    static Unique Create(Args&&... args)
    {
        return Unique(new T(std::forward<Args>(args)...));
    }

private:
    T* m_data = nullptr;
};

}
}
