#include "trpch.h"
#include "Core/UUID.h"

#include <CoreFoundation/CFUUID.h>

namespace TerranEngine 
{
	// TODO: implment function, currently not implemented due to having linking errors
	// add an ifdef at the top of the file so its only included in macos builds
	void UUID::Generate()
	{
		CFUUIDRef uuid = CFUUIDCreate(NULL);
		CFUUIDBytes bytes = CFUUIDGetUUIDBytes(uuid);
		
		m_Data = 
		{
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
}

