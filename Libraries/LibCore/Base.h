#pragma once

#include <memory>
/**
 * @ingroup LibCore
 * @brief Binds a member function to the current instance.
 *
 * @details
 * Convenience macro used primarily for event or callback systems.
 * Expands to `std::bind_front(&event, this)`.
 *
 * @param event Member function to bind
 */
#define TR_EVENT_BIND_FN(event) std::bind_front(&event, this)

/**
 * @ingroup LibCore
 * @brief Converts a token into a string literal.
 *
 * @param a Token to stringify
 */
#define TR_STRINGIFY(a) #a

/**
 * @ingroup LibCore
 * @brief Returns the name of a symbol as a string literal.
 *
 * @details
 * This macro is functionally identical to `TR_STRINGIFY` and is
 * intended for semantic clarity.
 *
 * @param a Symbol to stringify
 */
#define TR_NAMEOF(a) #a


/**
 * @ingroup LibCore
 * @brief Marks a declaration as deprecated with a custom message.
 *
 * @param message Deprecation message shown to the user
 */
#define TR_DEPRICATED(message) [[deprecated(message)]]

namespace Terran::Core {

/**
 * @ingroup LibCore
 * @brief Shared ownership smart pointer alias.
 *
 * @tparam T Managed type
 */
template<typename T>
using Shared = std::shared_ptr<T>;
/**
 * @ingroup LibCore
 * @brief Creates a Shared pointer using forwarding.
 *
 * @tparam T    Type to construct
 * @tparam Args Constructor argument types
 *
 * @param args Arguments forwarded to the constructor
 * @return Shared pointer managing the newly created object
 */
template<typename T, typename... Args>
Shared<T> CreateShared(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

/**
 * @ingroup LibCore
 * @brief Weak (non-owning) smart pointer alias.
 *
 * @tparam T Referenced type
 */
template<typename T>
using Weak = std::weak_ptr<T>;

/**
 * @ingroup LibCore
 * @brief Performs a dynamic cast on a Shared pointer.
 *
 * @tparam T1 Target type
 * @tparam T2 Source type
 *
 * @param ptr Source shared pointer
 * @return Shared pointer of type T1, or nullptr if the cast fails
 */
template<typename T1, typename T2>
Shared<T1> DynamicCast(Shared<T2> const& ptr)
{
    return std::dynamic_pointer_cast<T1, T2>(ptr);
}

/**
 * @ingroup LibCore
 * @brief Non-owning raw pointer view into an object.
 *
 * @details
 * This alias is used to explicitly indicate that the pointer does not
 * own the referenced object.
 *
 * @tparam T Pointed-to type
 */
template<typename T>
using RawPtr = T*;

}

namespace Literals {

/**
 * @ingroup LibCore
 * @brief Literal for kilobytes.
 *
 * @param x Value in kilobytes
 * @return Value converted to bytes
 */
inline unsigned long long operator""_kb(unsigned long long const x)
{
    return x * 1024ull;
}

/**
 * @ingroup LibCore
 * @brief User-defined literal for megabytes.
 *
 * @param x Value in megabytes
 * @return Value converted to bytes
 */
inline unsigned long long operator""_mb(unsigned long long const x)
{
    return x * (1024ull * 1024ull);
}

}
