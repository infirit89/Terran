#pragma once

namespace Terran {
namespace Core {

template<typename T>
struct DefaultDelete {
    constexpr DefaultDelete() noexcept = default;
    constexpr void operator()(T* value) noexcept
    {
        delete value;
    }
};

template<typename T>
struct DefaultDelete<T[]> {

    constexpr DefaultDelete() noexcept = default;
    constexpr void operator()(T* value) noexcept
    {
        delete[] value;
    }
};

}
}
