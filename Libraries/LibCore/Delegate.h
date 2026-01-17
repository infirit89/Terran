#pragma once

#include <type_traits>
#include <functional>

namespace Terran::Core {
template<typename T>
class Delegate final {};

/**
 * @brief Delegate class that currently only supports member functions and free functions
 *
 * @note Currently does not support lambdas or functors
 */
template<typename Ret, typename... Args>
class Delegate<Ret (Args...)> final {
    using storage_type = void*;
    using fn_type = Ret(storage_type, Args...);

public:
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

    template<auto TFunction>
    requires(std::is_invocable_r_v<Ret, decltype(TFunction), Args...>)
    void connect() {
        m_storage = nullptr;
        m_fn = [](storage_type, Args... args) -> Ret {
            return std::invoke(TFunction, args...);
        };
    }

    Ret operator()(Args... args) const {
        return m_fn(m_storage, args...);
    }
private:
    fn_type* m_fn;
    storage_type m_storage;
};
}
