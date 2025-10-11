#include "UUID.h"

#include <array>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <string>

namespace Terran {
namespace Core {

UUID UUID::s_Empty;

UUID::UUID()
    : m_Data({})
{
    Generate();
}

UUID::UUID(std::array<uint8_t, 16> const& data)
    : m_Data(data)
{
}

UUID UUID::CreateFromRaw(uint8_t const* data)
{
    std::array<uint8_t, 16> idData = {};
    constexpr uint32_t uuidArrayLength = 16 * sizeof(uint8_t);

    memcpy(&idData, data, uuidArrayLength);

    return UUID({ idData });
}

static unsigned char CharToHex(char const& ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'a' && ch <= 'f')
        return 10 + ch - 'a';
    if (ch >= 'A' && ch <= 'F')
        return 10 + ch - 'A';

    return 0;
}

UUID UUID::FromString(std::string const& str)
{
    size_t index = 0;

    std::array<uint8_t, 16> data { {} };

    if (str.empty())
        return UUID({ 0 });

    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '-')
            continue;

        if (index >= 16 || !isxdigit(str[i])) {
            // TODO: return not in hex format error
            return UUID({ 0 });
        }

        data[index] = static_cast<uint8_t>(CharToHex(str[i]) << 4);
        data[index] |= (uint8_t)(CharToHex(str[++i]));

        index++;
    }

    if (index < 16)
        return UUID();

    return UUID { data };
}

}

}
