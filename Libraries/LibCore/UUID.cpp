#include "UUID.h"
#include "PlatformCheck.h"
#include "Base.h"

#include <array>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <string>

namespace Terran::Core {

const UUID UUID::s_empty = UUID(UUID::type { { 0 } });
// 00 00 00 00 - 00 00 - 00 00 - 00 00 - 00 00 00 00 00 00
// 16 bytes = 32 characters (for values)
// 32 characters + 4 (dashes) = 36 (total characters)
constexpr size_t s_uuid_string_size = 36;

UUID UUID::from_string(std::string const& str)
{
    size_t index = 0;

    std::array<uint8_t, 16> data { {} };

    if (str.size() != s_uuid_string_size)
        return UUID({ 0 });

    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '-')
            continue;

        if (!isxdigit(str[i]) || !isxdigit(str[i + 1])) {
            // TODO: return not in hex format error
            return UUID({ 0 });
        }

        data[index] = static_cast<uint8_t>(char_to_hex(str[i]) << 4);
        data[index] |= (uint8_t)(char_to_hex(str[++i]));

        index++;
    }


    return UUID { data };
}

#ifdef TR_MACOSX
#    include <CoreFoundation/CFUUID.h>
UUID::type UUID::generate()
{
    CFUUIDRef uuid = CFUUIDCreate(NULL);
    CFUUIDBytes bytes = CFUUIDGetUUIDBytes(uuid);

    return {
        {
            bytes.byte0,
            bytes.byte1,
            bytes.byte2,
            bytes.byte3,
            bytes.byte4,
            bytes.byte5,
            bytes.byte6,
            bytes.byte7,
            bytes.byte8,
            bytes.byte9,
            bytes.byte10,
            bytes.byte11,
            bytes.byte12,
            bytes.byte13,
            bytes.byte14,
            bytes.byte15,
        }
    };
}
#elif defined(TR_WINDOWS)
#    pragma comment(lib, "rpcrt4.lib")
#    include <rpc.h>

UUID::type UUID::generate()
{
    GUID uuid;
    UuidCreate(&uuid);

    // based on: https://github.com/mariusbancila/stduuid/blob/master/include/uuid.h , class uuid_system_generator, line 606

    /* Windows _GUID struct:
     *  unsigned long
     *  unsigned short
     *  unsigned short
     *  unsigned char[8]
     */

    return { { // extract the long
        static_cast<uint8_t>((uuid.Data1 >> 24) & 0xFF),
        static_cast<uint8_t>((uuid.Data1 >> 16) & 0xFF),
        static_cast<uint8_t>((uuid.Data1 >> 8) & 0xFF),
        static_cast<uint8_t>((uuid.Data1) & 0xFF),

        // extract the first short
        static_cast<uint8_t>((uuid.Data2 >> 8) & 0xFF),
        static_cast<uint8_t>((uuid.Data2) & 0xFF),

        // extract the second short
        static_cast<uint8_t>((uuid.Data3 >> 8) & 0xFF),
        static_cast<uint8_t>((uuid.Data3) & 0xFF),

        // the char array
        (uint8_t)uuid.Data4[0],
        (uint8_t)uuid.Data4[1],
        (uint8_t)uuid.Data4[2],
        (uint8_t)uuid.Data4[3],
        (uint8_t)uuid.Data4[4],
        (uint8_t)uuid.Data4[5],
        (uint8_t)uuid.Data4[6],
        (uint8_t)uuid.Data4[7] } };

    m_Data = data;
}
#endif

}
