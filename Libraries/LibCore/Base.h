#pragma once

#include <memory>

#define TR_EVENT_BIND_FN(event) std::bind_front(&event, this)
#define TR_STRINGIFY(a) #a
#define TR_DEPRICATED(message) [[deprecated(message)]]

namespace Terran {
namespace Core {

template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T, typename... Args>
Shared<T> CreateShared(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Weak = std::weak_ptr<T>;

template<typename T1, typename T2>
Shared<T1> DynamicCast(Shared<T2> const& ptr)
{
    return std::dynamic_pointer_cast<T1, T2>(ptr);
}

// Used to indicate a view into a smart pointer
template<typename T>
using RawPtr = T*;

}

namespace Literals {

inline unsigned long long operator""_kb(unsigned long long const x)
{
    return x * 1024ull;
}

inline unsigned long long operator""_mb(unsigned long long const x)
{
    return x * (1024ull * 1024ull);
}

}
}
