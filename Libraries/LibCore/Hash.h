#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace Terran {
namespace Core {

class Hash final {
public:
    static uint32_t fnv1a(std::string const& str);
    static uint32_t fnv1a(std::string_view str);
    static uint64_t fnv1a_64(std::string_view str);
};

}

}
