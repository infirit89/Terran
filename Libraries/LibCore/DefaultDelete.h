/**
 * @file DefaultDelete.h
 * @brief Default delete specilizations for things like @see Unique.
 * @ingroup LibCore
 */
#pragma once

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

}
}
