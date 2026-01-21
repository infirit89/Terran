/**
 * @file Hash.h
 * @brief Provides hashing utility functions
 * @ingroup LibCore
 */
#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace Terran::Core {

/**
 * @class Hash
 * @brief Provides hashing utilities
 */
class Hash final {
public:

    /**
     * @brief Generate a 32 bit FNV1a hash for a string
     *
     * @param str The string for which to generate a hash
     * @returns 32 bit fnv1a hash
     */
    static uint32_t fnv1a(std::string const& str);

    /**
     * @brief Generate a 32 bit FNV1a hash for a string_view
     *
     * @param str The string_view for which to generate a hash
     * @returns 32 bit fnv1a hash
     */
    static uint32_t fnv1a(std::string_view str);

    /**
     * @brief Generate a 64 bit FNV1a hash for a string_view
     *
     * @param str The string_view for which to generate a hash
     * @returns 64 bit fnv1a hash
     */
    static uint64_t fnv1a_64(std::string_view str);
};

}
