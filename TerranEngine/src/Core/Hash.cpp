#include "trpch.h"
#include "Hash.h"

namespace TerranEngine 
{
    uint32_t Hash::FNVHash(const std::string& str)
    {
        return FNVHash(std::string_view(str));
    }

    uint32_t Hash::FNVHash(std::string_view str)
    {
        const uint32_t FNV32Prime = 16777619;
        const uint32_t FNV32Offset = 2166136261;

        uint32_t hash = FNV32Offset;

        for (char c : str)
        {
            hash = hash ^ c;
            hash = hash * FNV32Prime;
        }

        return hash;
    }
}

