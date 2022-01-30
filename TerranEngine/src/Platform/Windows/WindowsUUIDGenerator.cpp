#include "trpch.h"
#include "Core/UUID.h"

#pragma comment(lib, "rpcrt4.lib")
#include <rpc.h>

#ifdef TR_WINDOWS_64

namespace TerranEngine 
{
    void UUID::Generate()
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

        std::array<uint8_t, 16> data =
        { {
              // extract the long
              (uint8_t)((uuid.Data1 >> 24) & 0xFF),
              (uint8_t)((uuid.Data1 >> 16) & 0xFF),
              (uint8_t)((uuid.Data1 >> 8) & 0xFF),
              (uint8_t)((uuid.Data1) & 0xFF),

              // extract the first short
              (uint8_t)((uuid.Data2 >> 8) & 0xFF),
              (uint8_t)((uuid.Data2) & 0xFF),

              // extract the second short
              (uint8_t)((uuid.Data3 >> 8) & 0xFF),
              (uint8_t)((uuid.Data3) & 0xFF),

              // the char array
              (uint8_t)uuid.Data4[0],
              (uint8_t)uuid.Data4[1],
              (uint8_t)uuid.Data4[2],
              (uint8_t)uuid.Data4[3],
              (uint8_t)uuid.Data4[4],
              (uint8_t)uuid.Data4[5],
              (uint8_t)uuid.Data4[6],
              (uint8_t)uuid.Data4[7]
           } };

        m_Data = data;
	}
}

#endif // TR_WINDOWS_64
