#pragma once

#include <LibCore/DefaultDelete.h>
#include <atomic>
#include <cstddef>
#include <type_traits>
#include <utility>
namespace Terran::Core {

class RefCounted {
public:
    virtual ~RefCounted() = default;

    void increment_count()
    {
        m_reference_count++;
    }

    void decrement_count()
    {
        m_reference_count--;
    }

    size_t ref_count() const
    {
        return m_reference_count.load();
    }

private:
    std::atomic_size_t m_reference_count = 0;
};

template<typename T>
concept IsRefCounted = std::is_base_of_v<RefCounted, T>;

template<IsRefCounted TValue>
class RefPtr final {
    using value_type = TValue;

public:
    constexpr RefPtr() noexcept = default;
    constexpr RefPtr(std::nullptr_t) noexcept
        : m_data(nullptr)
    {
    }

    constexpr RefPtr(TValue* data) noexcept
        : m_data(data)
    {
        increment_ref();
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    constexpr RefPtr(TYValue* data) noexcept
        : m_data(data)
    {
        increment_ref();
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    constexpr RefPtr(RefPtr<TYValue> const& other) noexcept
        : m_data(other.m_data)
    {
        increment_ref();
    }

    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    constexpr RefPtr(RefPtr<TYValue>&& other) noexcept
        : m_data(other.release())
    {
    }

    constexpr RefPtr(RefPtr const& other) noexcept
        : m_data(other.m_data)
    {
        increment_ref();
    }

    constexpr RefPtr(RefPtr&& other) noexcept
        : m_data(other.release())
    {
    }

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
        if (this == other)
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
        if (this == other)
            return *this;

        other.increment_ref();
        decrement_ref();
        m_data = other.m_data;
        return *this;
    }

    constexpr RefPtr& operator=(RefPtr&& other) noexcept
    {
        decrement_ref();
        m_data = other.m_data;
        other.m_data = nullptr;
        return *this;
    }
    template<IsRefCounted TYValue>
    requires(std::is_convertible_v<TYValue*, TValue*>)
    constexpr RefPtr& operator=(RefPtr<TYValue>&& other) noexcept
    {
        decrement_ref();
        m_data = other.m_data;
        other.m_data = nullptr;
        return *this;
    }

    constexpr RefPtr& operator=(TValue* data) noexcept {
        m_data = data;
        increment_ref();
        return *this;
    }

    constexpr operator bool() const noexcept
    {
        return !is_null();
    }

    constexpr bool is_null() const noexcept
    {
        return !m_data;
    }

    template<typename... TArgs>
    static constexpr RefPtr create(TArgs&&... args) {
        return RefPtr(new TValue(std::forward<TArgs>(args)...));
    }

private:
    void increment_ref()
    {
        if (m_data) {
            m_data->increment_count();
        }
    }

    void decrement_ref()
    {
        if (!m_data) {
            return;
        }

        m_data->decrement_count();

        if (m_data->ref_count() == 0) {
            delete m_data;
            m_data = nullptr;
        }
    }

private:
    value_type* m_data = nullptr;
};

template<IsRefCounted TValue, IsRefCounted TYValue>
constexpr RefPtr<TValue> static_pointer_cast(RefPtr<TYValue> const& other) {
    return RefPtr<TValue>(static_cast<TValue*>(other.data()));
}

template<IsRefCounted TValue, IsRefCounted TYValue>
constexpr RefPtr<TValue> dynamic_pointer_cast(RefPtr<TYValue> const& other) {
    return RefPtr<TValue>(dynamic_cast<TValue*>(other.data()));
}

}
