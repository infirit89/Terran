#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace Terran {
namespace Core {

class Hash final {
public:
    static uint32_t FNVHash(std::string const& str);
    static uint32_t FNVHash(std::string_view str);
};

}

}
