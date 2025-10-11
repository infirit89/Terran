#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace Terran {
namespace Core {

class UUID {
public:
    UUID();
    UUID(std::array<uint8_t, 16> const& data);
    UUID(const UUID& other) = default;
    static UUID Invalid() { return s_Empty; }

    std::array<uint8_t, 16> GetData() const { return m_Data; }
    std::array<uint8_t, 16> GetData() { return m_Data; }

    void SetData(std::array<uint8_t, 16> const& idArray) { m_Data = idArray; }

    uint8_t const* GetRaw() const { return m_Data.data(); }
    static UUID CreateFromRaw(uint8_t const* data);

    bool operator==(const UUID& other) const { return m_Data == other.m_Data; }
    bool operator==(const UUID& other) { return m_Data == other.m_Data; }

    bool operator!=(const UUID& other) const { return m_Data != other.m_Data; }
    bool operator!=(const UUID& other) { return m_Data != other.m_Data; }

    bool operator<(const UUID& other) const { return m_Data < other.m_Data; }
    bool operator<(const UUID& other) { return m_Data < other.m_Data; }

    bool operator>(const UUID& other) const { return m_Data > other.m_Data; }
    bool operator>(const UUID& other) { return m_Data > other.m_Data; }

    operator bool() const { return IsValid(); }

    bool IsValid() const
    {
        std::array<uint8_t, 16> empty { { 0 } };

        return m_Data != empty;
    }

    static UUID FromString(std::string const& str);

private:
    void Generate();
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
    std::array<uint8_t, 16> m_Data;
    static UUID s_Empty;
};

}

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

        std::array<uint8_t, 16> idArr = uuid.GetData();

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
    std::array<uint8_t, 16> idArr = id.GetData();

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
