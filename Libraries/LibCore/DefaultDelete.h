/**
 * @file DefaultDelete.h
 * @brief Default delete specilizations for things like @see Unique.
 * @ingroup LibCore
 */
#pragma once

#include <concepts>
namespace Terran {
namespace Core {

/**
 * @struct DefaultDelete
 * @brief Used to specialize a default deletion function for other utilities
 *
 * @tparam T The type of object to delete
 */
template<typename T>
struct DefaultDelete {
    constexpr DefaultDelete() noexcept = default;

    constexpr void operator()(T const* value) noexcept
    {
        delete value;
    }

    constexpr void operator()(T* value) noexcept
    {
        delete value;
    }
};

/**
 * @struct DefaultDelete<T[]>
 * @brief Used to specialize a default array deletion function for other utilities
 *
 * @tparam T The type of array to delete
 */
template<typename T>
struct DefaultDelete<T[]> {

    constexpr DefaultDelete() noexcept = default;
    constexpr void operator()(T* value) noexcept
    {
        delete[] value;
    }
};

template<typename T>
concept IsDefaultDelete = requires(T t, T* ptr, T const* const_ptr) {
    { t(ptr) } -> std::same_as<void> ;
    { t(const_ptr) } -> std::same_as<void> ;

};

}
}
