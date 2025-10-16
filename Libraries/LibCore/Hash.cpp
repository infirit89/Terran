#include "Hash.h"

#include <cstdint>
#include <string>
#include <string_view>

namespace Terran {
namespace Core {

uint32_t Hash::FNVHash(std::string const& str)
{
    return FNVHash(std::string_view(str));
}

uint32_t Hash::FNVHash(std::string_view str)
{
    constexpr uint32_t const FNV32Prime = 16777619;
    constexpr uint32_t const FNV32Offset = 2166136261;

    uint32_t hash = FNV32Offset;

    for (char c : str) {
        hash = hash ^ c;
        hash = hash * FNV32Prime;
    }

    return hash;
}

}

}
