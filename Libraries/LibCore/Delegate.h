#pragma once

#include <type_traits>
#include <functional>

namespace Terran::Core {
template<typename T>
class Delegate final {};

/**
 * @ingroup LibCore
 * @brief Type-erased callable wrapper for free and member functions.
 *
 * @details
 * Delegate provides a lightweight, non-owning function wrapper that
 * supports:
 * - Free functions
 * - Member functions (via an explicitly provided instance)
 *
 * Internally, the delegate stores:
 * - A pointer-sized storage value (typically used to hold an object instance)
 * - A function pointer that knows how to invoke the stored callable
 *
 * ### Limitations
 * - Lambdas are not supported
 * - Functors are not supported
 * - No lifetime management is performed
 * - Only trivially copyable, trivially destructible instance types are allowed
 *
 * @note This class does not perform null checks. Invoking an unconnected
 * delegate results in undefined behavior.
 *
 * @tparam Ret  Return type of the callable
 * @tparam Args Argument types of the callable
 */
template<typename Ret, typename... Args>
class Delegate<Ret (Args...)> final {
    using storage_type = void*;
    using fn_type = Ret(storage_type, Args...);

public:

    /**
     * @brief Connects a member function to the delegate.
     *
     * @details
     * Binds a member function to a specific instance. The instance is
     * stored by value inside the delegate’s internal storage.
     *
     * The instance type must:
     * - Fit into a pointer-sized storage
     * - Be trivially copyable
     * - Be trivially destructible
     *
     * @tparam TFunction Member function pointer to bind
     * @tparam Type      Type of the instance
     *
     * @param instance Instance on which the member function will be invoked
     *
     * @note The instance is copied into internal storage. No lifetime
     * tracking is performed.
     */
    template<auto TFunction, typename Type>
    requires(
        sizeof(Type) <= sizeof(storage_type) && std::is_trivially_copyable_v<Type> && std::is_trivially_destructible_v<Type>
        && std::is_invocable_r_v<Ret, decltype(TFunction), Type&, Args...>)
    void connect(Type instance) {
        m_storage = instance;
        m_fn = [](storage_type storage, Args... args) -> Ret {
            Type instance = static_cast<Type>(storage);
            return std::invoke(TFunction, instance, args...);
        };
    }

    /**
     * @brief Connects a free (non-member) function to the delegate.
     *
     * @details
     * Binds a function that does not require an instance.
     * The internal storage is set to nullptr.
     *
     * @tparam TFunction Free function pointer to bind
     */
    template<auto TFunction>
    requires(std::is_invocable_r_v<Ret, decltype(TFunction), Args...>)
    void connect() {
        m_storage = nullptr;
        m_fn = [](storage_type, Args... args) -> Ret {
            return std::invoke(TFunction, args...);
        };
    }

    /**
     * @brief Invokes the bound callable.
     *
     * @param args Arguments forwarded to the stored function
     * @return Return value of the callable
     *
     * @warning Calling this operator without a previously connected
     * function results in undefined behavior.
     */
    Ret operator()(Args... args) const {
        return m_fn(m_storage, args...);
    }
private:
    fn_type* m_fn;
    storage_type m_storage;
};
}
