/**
 * @file UUID.h
 * @brief Identifier for all engine resources
 * @ingroup LibCore
 */
#pragma once

#include <array>
#include <cstdint>
#include <ios>
#include <string>

namespace Terran::Core {

/**
 * @class UUID
 * @brief Universally unique identifier, used for all engine resources
 */
class UUID final {
public:
    using type = std::array<uint8_t, 16>;
    /**
     * @brief Generate a new UUID
     */
    UUID()
        : m_data(generate())
    {
    }

    constexpr UUID(type const& data) noexcept
        : m_data(data)
    {
    }

    constexpr UUID(const UUID& other) noexcept
        : m_data(other.m_data)
    {
    }

    constexpr UUID(UUID&& other) noexcept
    {
        m_data = other.m_data;
        other.m_data = {};
    }

    ~UUID() = default;

    /**
     * @returns Invalid id
     */
    static constexpr const UUID& invalid() noexcept { return s_empty; }

    type constexpr data() const noexcept { return m_data; }

    void set_data(type const& data) { m_data = data; }

    constexpr type::value_type const* data_raw() const noexcept
    {
        return m_data.data();
    }

    /**
     * @brief Convert a string to an UUID
     * @retuns The converted id from the string
     */
    static UUID from_string(std::string const& str);

    constexpr UUID& operator=(const UUID& other) noexcept
    {
        return *this = UUID(other);
    }

    constexpr UUID& operator=(UUID&& other) noexcept
    {
        m_data = other.m_data;
        other.m_data = {};
        return *this;
    }

    constexpr bool operator==(const UUID& other) const noexcept
    {
        return m_data == other.m_data;
    }

    constexpr bool operator!=(const UUID& other) const noexcept
    {
        return m_data != other.m_data;
    }

    constexpr bool operator<(const UUID& other) const noexcept
    {
        return m_data < other.m_data;
    }

    constexpr bool operator>(const UUID& other) const noexcept
    {
        return m_data > other.m_data;
    }

    constexpr operator bool() const noexcept
    {
        return is_valid();
    }

    constexpr bool is_valid() const noexcept
    {
        return m_data != s_empty.m_data;
    }

private:
    [[nodiscard]] static type generate();
    /*
     * empty UUID:
     * 1	 2  3  4    5  6    7  8    9  10   11 12 13 14 15 16
     * 00 00 00 00 - 00 00 - 00 00 - 00 00 - 00 00 00 00 00 00
     *
     * 1-4: time-low
     * 5-6: time-mid
     *
     * 7-8: time-high and version
     *
     * 9: clock-seq and reserved
     *
     * 10: clock-seq low
     *
     * 11-16: node
     */
    type m_data;
    static const UUID s_empty;
};

}

template<typename OStream>
OStream& operator<<(OStream& os, Terran::Core::UUID const& uuid)
{
    std::array<uint8_t, 16> idArr = uuid.data();

    int ind = 0;
    for (int i = 0; i < 20; i++) {
        if (i == 4 || i == 7 || i == 10 || i == 13) {
            os << '-';
            continue;
        }

        os << std::hex
           << (idArr[ind] >> 4 & 0x0f)
           << (int)(idArr[ind] & 0x0f);

        ind++;
    }

    return os;
}

#include <cstddef>
#include <iostream>
#include <sstream>

namespace std {

template<>
struct hash<Terran::Core::UUID> {
    size_t operator()(Terran::Core::UUID const& uuid) const noexcept
    {
        std::hash<uint8_t> hash;

        size_t result = 0;

        std::array<uint8_t, 16> idArr = uuid.data();

        for (size_t i = 0; i < idArr.size(); i++)
            result = (result << 1) ^ hash(idArr[i]);

        return result;
    }
};

template<class CharT = char,
    class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT>>
std::basic_string<CharT, Traits, Allocator> to_string(Terran::Core::UUID const& id)
{
    std::array<uint8_t, 16> idArr = id.data();

    std::basic_stringstream<CharT, Traits, Allocator> sstr;

    int ind = 0;
    for (int i = 0; i < 20; i++) {
        if (i == 4 || i == 7 || i == 10 || i == 13) {
            sstr << '-';
            continue;
        }

        sstr << std::hex
             << (idArr[ind] >> 4 & 0x0f)
             << (int)(idArr[ind] & 0x0f);

        ind++;
    }

    return sstr.str();
}

}
