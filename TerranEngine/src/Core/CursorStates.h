#pragma once

#include <cstdint>

namespace TerranEngine 
{
	enum class CursorState : uint32_t
	{
		Arrow		= 0x00036001,
		IBeam		= 0x00036002,
		Crosshair	= 0x00036003,
		Hand		= 0x00036004,
		HResize		= 0x00036005,
		VResize		= 0x00036006
	};
}