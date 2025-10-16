#pragma once

namespace Terran {
namespace Core {

static constexpr unsigned char char_to_hex(char const& ch) noexcept
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'a' && ch <= 'f')
        return 10 + ch - 'a';
    if (ch >= 'A' && ch <= 'F')
        return 10 + ch - 'A';

    return 0;
}

}
}
