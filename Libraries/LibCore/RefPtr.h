#pragma once

#include "DefaultDelete.h"
#include "RefCounted.h"
#include "Unique.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace Terran::Core {


template<IsRefCounted TValue>
class RefPtr final {
    using value_type = TValue;

public:
    constexpr RefPtr() noexcept = default;
    explicit(false) constexpr RefPtr(std::nullptr_t) noexcept
    {
    }

    explicit(false) constexpr RefPtr(TValue* data) noexcept
        : m_data(data)
    {
        increment_ref();
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    explicit(false) constexpr RefPtr(TYValue* data) noexcept
        : m_data(data)
    {
        increment_ref();
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    explicit(false) constexpr RefPtr(RefPtr<TYValue> const& other) noexcept
        : m_data(other.m_data)
    {
        increment_ref();
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    explicit(false) constexpr RefPtr(RefPtr<TYValue>&& other) noexcept
        : m_data(other.release())
    {
    }

    explicit(false) constexpr RefPtr(RefPtr const& other) noexcept
        : m_data(other.m_data)
    {
        increment_ref();
    }

    explicit(false) constexpr RefPtr(RefPtr&& other) noexcept
        : m_data(other.release())
    {
    }

    template<IsRefCounted TYValue>
    RefPtr(Unique<TYValue> const& other) = delete;

    constexpr ~RefPtr()
    {
        reset();
    }

    constexpr void reset() noexcept
    {
        decrement_ref();
    }

    [[nodiscard]] constexpr value_type* release() noexcept
    {
        return std::exchange(m_data, nullptr);
    }

    [[nodiscard]] constexpr value_type* data() const noexcept
    {
        return m_data;
    }

    void swap(RefPtr& other) noexcept
    {
        std::swap(m_data, other.m_data);
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    void swap(RefPtr<TYValue>& other) noexcept
    {
        std::swap(m_data, other.m_data);
    }

    constexpr bool operator==(RefPtr const& other) const noexcept
    {
        return m_data == other.m_data;
    }

    constexpr value_type* operator->() const noexcept
    {
        return m_data;
    }

    constexpr RefPtr& operator=(std::nullptr_t) noexcept
    {
        decrement_ref();
        m_data = nullptr;
        return *this;
    }

    constexpr RefPtr& operator=(RefPtr const& other) noexcept
    {
        if (this == &other)
            return *this;

        other.increment_ref();
        decrement_ref();
        m_data = other.m_data;
        return *this;
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    constexpr RefPtr& operator=(RefPtr<TYValue> const& other) noexcept
    {
        if (this == &other)
            return *this;

        other.increment_ref();
        decrement_ref();
        m_data = other.m_data;
        return *this;
    }

    constexpr RefPtr& operator=(RefPtr&& other) noexcept
    {
        // NOTE: this cleans up memory because temp gets destroyed at the end of the scope
        RefPtr temp { std::move(other) };
        swap(temp);
        return *this;
    }
    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    constexpr RefPtr& operator=(RefPtr<TYValue>&& other) noexcept
    {
        RefPtr temp { std::move(other) };
        swap(temp);
        return *this;
    }

    constexpr RefPtr& operator=(TValue* data) noexcept
    {
        m_data = data;
        increment_ref();
        return *this;
    }

    template<IsRefCounted TYValue>
    RefPtr& operator=(Unique<TYValue> const& other) = delete;

    explicit(false) constexpr operator bool() const noexcept
    {
        return !is_null();
    }

    constexpr bool is_null() const noexcept
    {
        return !m_data;
    }

    template<typename... TArgs>
    static constexpr RefPtr create(TArgs&&... args)
    {
        return RefPtr(new TValue(std::forward<TArgs>(args)...));
    }

private:
    void increment_ref() const
    {
        if (m_data) {
            m_data->increment_count();
        }
    }

    void decrement_ref() const
    {
        if (!m_data) {
            return;
        }

        m_data->decrement_count();

        if (m_data->ref_count() == 0) {
            DefaultDelete<TValue> deleter;
            deleter(m_data);
            m_data = nullptr;
        }
    }

private:
    mutable value_type* m_data = nullptr;

    template<IsRefCounted TYValue>
    friend class RefPtr;
};

template<IsRefCounted TValue, IsRefCounted TYValue>
constexpr RefPtr<TValue> static_pointer_cast(RefPtr<TYValue> const& other)
{
    return RefPtr<TValue>(static_cast<TValue*>(other.data()));
}

template<IsRefCounted TValue, IsRefCounted TYValue>
constexpr RefPtr<TValue> dynamic_pointer_cast(RefPtr<TYValue> const& other)
{
    return RefPtr<TValue>(dynamic_cast<TValue*>(other.data()));
}

}
