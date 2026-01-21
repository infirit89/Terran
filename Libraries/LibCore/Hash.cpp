#include "Hash.h"

#include <cstdint>
#include <string>
#include <string_view>

namespace Terran::Core {

uint32_t Hash::fnv1a(std::string const& str)
{
    return fnv1a(std::string_view(str));
}

uint32_t Hash::fnv1a(std::string_view str)
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

uint64_t Hash::fnv1a_64(std::string_view str)
{
    constexpr uint64_t const FNV64Prime = 0x00000100000001b3;
    constexpr uint64_t const FNV64Offset = 0xcbf29ce484222325;

    uint64_t hash = FNV64Offset;

    for (char c : str) {
        hash = hash ^ c;
        hash = hash * FNV64Prime;
    }

    return hash;
}

}
