#pragma once

#include "DefaultDelete.h"

#include <atomic>
#include <cstddef>
#include <type_traits>

namespace Terran::Core {

// NOTE: meant to be used ONLY internally by RefCounted, RefPtr and WeakPtr
// meant to be allocated on the heap
class WeakData {

public:
    void increment_weak_count() const
    {
        m_weak_count++;
    }

    void decrement_weak_count() const
    {
        m_weak_count--;
        if (weak_count() == 0) {
            DefaultDelete<WeakData> deleter;
            deleter(this);
        }
    }

    size_t weak_count() const
    {
        return m_weak_count.load();
    }

    bool is_alive() const noexcept
    {
        return m_is_alive.load();
    }

    void mark_dead() noexcept
    {
        m_is_alive.store(false);
    }

private:
    mutable std::atomic_size_t m_weak_count = 0;
    mutable std::atomic_bool m_is_alive;
};

class RefCounted {
public:
    virtual ~RefCounted()
    {
        if (m_weak_data) {
            m_weak_data->mark_dead();
            m_weak_data->decrement_weak_count();
        }
    }

    void increment_count() const
    {
        m_reference_count++;
    }

    void decrement_count() const
    {
        m_reference_count--;
    }

    size_t ref_count() const
    {
        return m_reference_count.load();
    }

    WeakData* acquire_weak_data() const
    {
        if (!m_weak_data) {
            m_weak_data = new WeakData();
        }
        return m_weak_data;
    }

private:
    mutable std::atomic_size_t m_reference_count = 0;
    mutable WeakData* m_weak_data = nullptr;
};

template<typename T>
concept IsRefCounted = std::is_base_of_v<RefCounted, T>;

}
