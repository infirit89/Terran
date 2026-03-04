#pragma once
#include "RefPtr.h"
#include <LibCore/RefCounted.h>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace Terran::Core {

template<IsRefCounted TValue>
class WeakPtr final {
    using value_type = TValue;

public:
    constexpr WeakPtr() noexcept = default;
    explicit(false) constexpr WeakPtr(WeakPtr const& other) noexcept
        : m_data(other.m_data)
        , m_weak_data(other.m_weak_data)
    {
        increment_weak_count();
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    explicit(false) constexpr WeakPtr(WeakPtr<TYValue> const& other) noexcept
        : m_data(other.m_data)
        , m_weak_data(other.m_weak_data)
    {
        increment_weak_count();
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    explicit(false) constexpr WeakPtr(RefPtr<TYValue> const& other) noexcept
        : m_data(other.data())
        , m_weak_data(other.data() ? other.data()->acquire_weak_data() : nullptr)
    {
    }

    explicit(false) constexpr WeakPtr(WeakPtr&& other) noexcept
        : m_data(other.release())
        , m_weak_data(std::exchange(other.m_weak_data, nullptr))
    {
        increment_weak_count();
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    explicit(false) constexpr WeakPtr(WeakPtr<TYValue>&& other) noexcept
        : m_data(other.release())
        , m_weak_data(std::exchange(other.m_weak_data, nullptr))
    {
        increment_weak_count();
    }

    constexpr ~WeakPtr() noexcept
    {
        decrement_weak_count();
    }

    [[nodiscard]] constexpr value_type* release() noexcept
    {
        return std::exchange(m_data, nullptr);
    }

    [[nodiscard]] constexpr bool expired() const noexcept
    {
        return !m_weak_data || !m_weak_data->is_alive();
    }

    [[nodiscard]] constexpr RefPtr<TValue> lock() noexcept
    {
        if (expired()) {
            return RefPtr<TValue>(nullptr);
        }

        return RefPtr<TValue>(m_data);
    }

    constexpr bool operator==(WeakPtr const& other) const noexcept
    {
        return m_data == other.m_data;
    }

    explicit(false) constexpr operator bool() const noexcept
    {
        return !is_null();
    }

    constexpr bool is_null() const noexcept
    {
        return !m_data;
    }

    constexpr WeakPtr& operator=(WeakPtr const& other) noexcept
    {
        if (this == other)
            return *this;

        m_data = other.m_data;
        return *this;
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    constexpr WeakPtr& operator=(WeakPtr<TYValue> const& other) noexcept
    {
        if (this == other)
            return *this;

        m_data = other.m_data;
        return *this;
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    constexpr WeakPtr& operator=(RefPtr<TYValue> const& other) noexcept
    {
        m_data = other.data();
        return *this;
    }

    constexpr WeakPtr& operator=(WeakPtr&& other) noexcept
    {
        WeakPtr temp { std::move(other) };
        m_data = temp.m_data;
        return *this;
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    constexpr WeakPtr& operator=(WeakPtr<TYValue>&& other) noexcept
    {
        WeakPtr temp { std::move(other) };
        m_data = temp.m_data;
        return *this;
    }

private:
    void increment_weak_count() const noexcept
    {
        if (m_weak_data) {
            m_weak_data->increment_weak_count();
        }
    }

    void decrement_weak_count() const noexcept
    {
        if (m_weak_data) {
            m_weak_data->decrement_weak_count();
        }
    }

private:
    value_type* m_data = nullptr;
    WeakData* m_weak_data = nullptr;
};

}
